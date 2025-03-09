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
    phase_choose_notetaker,
    phase_take_attendance,
    phase_read_minutes,
    phase_approve_agenda,
    phase_announcements,
    phase_reports,
    phase_old_business,
    phase_new_business,
    phase_good_and_welfare,
    phase_next_chair,
    phase_meeting_critique,
    phase_end
};

// State
struct {
    ImGuiTextBuffer log_buffer;
    f32 scale = 1.5;
    b32 initialized;
    Person *selected;
    Person *chair;
    s32 notetaker_idx = -1;
    Person *people_arr[3];
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
} __s;
#define sa_log_text ImGui::LogText
#define sa imgui


//
// Helper
//

char* get_role_or_name(Person *person) {
    if (person == __s.chair)
        return (char *)__s.chair_str;
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
}
void reset() {
    __s.selected = 0;
}



b32 basic_action(const char * str, Person * person, Phase phase) {
    if (imgui::Selectable(str)) {
        // The chair calls the meeting to order
        __s.log_buffer.appendf("%s %s\n", get_role_or_name(__s.selected), str);
        if(__s.selected == person) {
            if (__s.phase == phase-1) {
                procedure(1);
                __s.phase = phase;
                return true;
            } else if (__s.phase > phase) {
                __s.log_buffer.appendf("... AGAIN\n");
            } else {
                __s.log_buffer.appendf("... but it's not time for that yet.\n");
            }
        } else if (person) {
            __s.log_buffer.appendf("%s says, uhm, that's my job..\n", get_role_or_name(person));
        } else {
            __s.log_buffer.appendf("... but something feels wrong about it.\n");
        }
        procedure(-1);
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


    sa_begin({0,0}, {500,600}, "Rusty's Rules");
    ImGui::SetWindowFontScale(__s.scale);





    //
    // Draw people
    // 
    
    for (s32 idx = 0; idx < ArrayCount(__s.people_arr); ++idx) {
        Person *person = __s.people_arr[idx];
        if(person) {
            //sa::PushID(idx);
            if(sa::Button(person->name)){
                __s.selected = person;
                imgui::OpenPopup("actions-to-take");
            }
            if(__s.people_arr[idx] == __s.chair) {
                sa::SameLine();
                sa_text("(chair)");
            }
            //sa::PopID();
        }
    }

    //
    // Actions
    //
    if (ImGui::BeginPopup("actions-to-take")) {
        basic_action("calls the meeting to order", __s.chair, phase_call_to_order);
        basic_action("moves to choose notetaker", __s.chair, phase_choose_notetaker);
        basic_action("moves to take attendance", __s.chair, phase_take_attendance); 
        basic_action("moves to read the previous meeting notes", __s.chair, phase_read_minutes);
        basic_action("moves to approve this meeting's agenda", __s.chair, phase_approve_agenda);
        basic_action("moves to brief announcements", __s.chair, phase_announcements); 
        basic_action("moves to reports", __s.chair, phase_reports); 
        basic_action("moves to old business", __s.chair, phase_old_business); 
        basic_action("moves to new business", __s.chair, phase_new_business); 
        basic_action("moves to good and welfare", __s.chair, phase_good_and_welfare); 
        basic_action("moves to pick the next chair", __s.chair, phase_next_chair); 
        basic_action("moves to critique meeting", __s.chair, phase_meeting_critique); 
        if(basic_action("moves to adjorn", __s.chair, phase_end)) {
            __s.log_buffer.appendf("Meeting complete!");
        }
        ImGui::EndPopup();
    }

    sa_text("%s", __s.log_buffer.c_str());
    imgui::End();
    
}

