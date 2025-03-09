#ifdef _WIN32
#include <windows.h>
#define SOKOL_D3D11
#endif

#ifdef __APPLE__
#define SOKOL_METAL
#endif

#include <stdio.h>
#include <math.h>
#include <time.h>
#define _USE_MATH_DEFINES
#include "types.h"
#include "memory.h"

#include "renderer.cpp"


#define create_enum(name) name,
#define create_strings(name) #name,

enum Pronoun {
  pronoun_she,
  pronoun_he,
  pronoun_they,
  pronoun_it,
};

struct Person {
    char name[100];
    s32 tolerance; // minimum procedure
    s32 iu;
    Pronoun pronouns[5];
    struct {
        s32 pronouns;
        s32 chairing;
        s32 recording_secretary;
        s32 iu;
    } training;
};
enum Phase {
    phase_none,
    phase_call_to_order,
    phase_notetaker_choose,
    phase_notetaker_nominate,
    phase_notetaker_second,
    phase_notetaker_waited,
    phase_take_attendance,
    phase_read_minutes,
    phase_approve_agenda,
    phase_announcements,
    phase_reports,
    phase_old_business,
    phase_new_business,
    phase_good_and_welfare,
    phase_chair_choose,
    phase_chair_nomination,
    phase_chair_second,
    phase_meeting_critique,
    phase_end
};

#define people_max 3

// State
struct {
    b32 scroll_down;
    ImGuiTextBuffer log_buffer;
    f32 scale = 2.0;
    b32 initialized;
    Person *selected;
    Person *nominated;
    Person *notetaker;
    Person *chair;
    s32 notetaker_idx = -1;
    Person *people_arr[people_max];
    struct {
        s32 inclusion = 0;
        s32 iww_knowledge = 0;
        s32 procedure = 0;
    }rating;
    Phase phase;
    b32 called_to_order = 0 ;
    b32 read_previous_meeting_notes;
    b32 choose_note_taker;
    char logs[200][200];
    s32 log_idx = 0;
    const char *chair_str = "The chair";
    const char *notetaker_str = "The notetaker";
} __s;
#define sa_log_text ImGui::LogText
#define sa imgui


//
// Helper
//

char* get_role_or_name(Person *person) {
    if (person == __s.chair) return (char *)__s.chair_str;
    if (person == __s.notetaker) return (char *)__s.notetaker_str;
    return (char *)person->name;
}

void clear_person(Person * person) {
    if (__s.chair == person) {
        __s.chair = NULL;
    }
    if (__s.selected == person) {
        __s.selected = NULL;
    }
}
s32 total_people() {
    s32 total = 0;

    for (s32 idx = 0; idx < ArrayCount(__s.people_arr); ++idx) {
        Person *person = __s.people_arr[idx];
        if (person) {
            total++;
        }
    }
    return total;
}

void procedure(s32 change) {
    __s.rating.procedure += change;
    char c[10] ={0};
    if (change > 0) {
        c[0] = '+';
    }
    __s.log_buffer.appendf("procedure %d (%s%d)\n", __s.rating.procedure, c, change);

    for (s32 idx = 0; idx < ArrayCount(__s.people_arr); ++idx) {
        Person *person = __s.people_arr[idx];
        if(person && __s.rating.procedure < person->tolerance) {
            if (total_people() > 1) {
                __s.log_buffer.appendf("%s gets up and storms out\n", get_role_or_name(person));
            } else {
                __s.log_buffer.appendf("%s, defeated, leaves dejectedly\n", get_role_or_name(person));
                __s.log_buffer.appendf("Without people, there can be no democracy. Game Over.\n");
            }
            clear_person(__s.people_arr[idx]);
            __s.people_arr[idx] = NULL;
        }
    }
    __s.selected = 0;
    __s.scroll_down = true;
}

b32 basic_action(const char * title, const char * str) {
    if (imgui::Selectable(title)) {
        // The chair calls the meeting to order
        __s.log_buffer.appendf("%s:\n%s\n", get_role_or_name(__s.selected), str);
        procedure(1);
        return true;
        procedure(-1);
    }
    return false;
}

b32 chair_action(const char * title, const char * str, Phase phase) {
    if (__s.phase == phase-1 && __s.selected == __s.chair) {
        if (imgui::Selectable(title)) {
            // The chair calls the meeting to order
            __s.log_buffer.appendf("%s:\n%s\n", get_role_or_name(__s.selected), str);
            procedure(1);
            __s.phase = phase;
            return true;
        }
    }
    return false;
}

b32 nominate(Phase phase) {
    if (__s.phase == phase-1) {
        for (s32 idx = 0; idx < people_max; ++idx) {
            Person *person2 = __s.people_arr[idx];
            //imgui::PushID(phase);
            //imgui::PushID(person2->name);
            char label[200];
            snprintf(label, 200, "Nominate %s", person2->name);
            if (imgui::Selectable(label)) {
                // The chair calls the meeting to order
                __s.log_buffer.appendf("%s:\nI nominate %s\n", get_role_or_name(__s.selected), person2->name);
                procedure(1);
                __s.phase = phase;
                __s.nominated = person2;
                return true;
            }
            //imgui::PopID();
            //imgui::PopID();
        }
    }
    return false;
}

b32 second_nomination(Person *position, Phase phase) {
    if (__s.phase == phase-1) {
        //imgui::PushID(phase);
        //imgui::PushID(person2->name);
        char label[200];
        snprintf(label, 200, "Second %s's Nomination", __s.nominated->name);
        if (imgui::Selectable(label)) {
            // The chair calls the meeting to order
            __s.log_buffer.appendf("%s:\nI second %s's nomination\n", get_role_or_name(__s.selected), __s.nominated->name);
            procedure(1);
            __s.phase = phase;
            position = __s.nominated;
            __s.nominated = NULL;
            return true;
        }
        //imgui::PopID();
        //imgui::PopID();
    }
    return false;
}
#define person_name_max 100

void create_person(Person * person, const char * name, s32 tolerance) {
    snprintf(person->name,person_name_max, "%s", name);
    person->tolerance = tolerance;
}
void game_loop() {

    // 
    // Initialize
    //
    
    if (!__s.initialized) {
        __s.initialized = 1;
        srand((u32)time(NULL));  


        imgui::GetStyle().ScaleAllSizes(__s.scale);
        // gets pixelated
        //ImGui::GetIO().FontGlobalScale = 1.5;
        
        // seth so dumb
        __s.people_arr[0] = (Person * )malloc(200);
        __s.people_arr[1] = (Person * )malloc(200);
        __s.people_arr[2] = (Person * )malloc(200);
        create_person(__s.people_arr[0], "James", -10);
        create_person(__s.people_arr[1], "Sam", -8);
        create_person(__s.people_arr[2], "Darius", -5);
        __s.chair = __s.people_arr[0];
    };


    sa_begin({0,0}, {screen.x,screen.y}, "Rusty's Rules");
    ImGui::SetWindowFontScale(__s.scale);




    imgui::BeginChild("People", {0, 5*imgui::GetFontSize()});

    //
    // Draw people
    // 
    
    for (s32 idx = 0; idx < ArrayCount(__s.people_arr); ++idx) {
        Person *person = __s.people_arr[idx];
        if(person) {
            //sa::PushID(idx);
            if(sa::Button(person->name)){
                __s.selected = person;
                //imgui::OpenPopup("actions-to-take");
            }
            if(__s.people_arr[idx] == __s.chair) {
                sa::SameLine();
                sa_text("(chair)");
            }
            if(__s.people_arr[idx] == __s.notetaker) {
                sa::SameLine();
                sa_text("(notetaker)");
            }
            //sa::PopID();
        }
    }

    //
    // Actions
    //

    imgui::EndChild();
    if (__s.selected) {
        ImGui::BeginChild("actions", {screen.x -30,18*imgui::GetFontSize()}, ImGuiChildFlags_Borders);
        chair_action("call to order", "I call this meeting to order at 6:00", phase_call_to_order);
        chair_action("choose notetaker", "I move to choose the notetaker", phase_notetaker_choose);
        chair_action("wait", "<waits>", phase_notetaker_nominate);
        nominate(phase_notetaker_nominate);
        if(chair_action("reject nomination", "Hearing none, the nomination is rejected", phase_notetaker_waited)){
            __s.nominated = NULL;
            __s.phase = phase_notetaker_choose;
        }
        second_nomination(__s.notetaker, phase_notetaker_second);
        chair_action("attendenance", "I move to take attendance", phase_take_attendance); 
        chair_action("previous meeting notes", "I move to read the previous meeting notes", phase_read_minutes);
        chair_action("previous meeting notes", "I move to approve the previous meeting notes.. all those in favor say aye.", phase_read_minutes);
        chair_action("approve agenda", "moves to approve this meeting's agenda", phase_approve_agenda);
        chair_action("announcements", "moves to brief announcements", phase_announcements); 
        chair_action("reports", "moves to reports", phase_reports); 
        chair_action("old business", "moves to old business", phase_old_business); 
        chair_action("new business", "moves to new business", phase_new_business); 
        chair_action("good and welfare", "moves to good and welfare", phase_good_and_welfare); 
        chair_action("pick next chair", "moves to pick the next chair", phase_chair_choose); 
        nominate(phase_chair_nomination);
        second_nomination(__s.notetaker, phase_chair_second);
        chair_action("critique meeting", "moves to critique meeting", phase_meeting_critique); 
        if(chair_action("adjorn", "I move to adjorn, all those in favor?", phase_end)) {
            __s.log_buffer.appendf("Meeting complete!");
        }

        basic_action("point of order", "says \"point of order\"");
        basic_action("point of information", "says \"point of information\"");
        basic_action("raise hand", "<Raises hand>");
        imgui::EndChild();
    } else {
        imgui::SeparatorText("Notes");
        ImGui::BeginChild("notes", {screen.x -30,0}, ImGuiChildFlags_Borders);
        if (__s.scroll_down) {
            __s.scroll_down = false;
            imgui::SetScrollFromPosY(10000);
        }
        imgui::TextWrapped("%s", __s.log_buffer.c_str());
        imgui::EndChild();
    }
    imgui::End();
    
}

