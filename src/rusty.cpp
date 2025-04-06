#ifdef _WIN32
#include <windows.h>
#define SOKOL_D3D11
#endif

#ifdef _WIN32
#include <windows.h>
#endif

#include <stddef.h>
#include <stdint.h>
#include <math.h>
#include <stdio.h>
#include <math.h>
#include <time.h>

#include "sokol/sokol_app.h"
#include "sokol/sokol_gfx.h"
#include "sokol/sokol_glue.h"
#include "sokol/sokol_log.h"
#include "rusty.h"
#include "imgui/imgui.h"
#include "sokol/util/sokol_imgui.h"
#include "rusty_render.h"

#include "rusty_render.cpp"

#define imgui ImGui


#define create_enum(name) name,
#define create_strings(name) #name,

enum Pronoun {
  pronoun_she,
  pronoun_he,
  pronoun_they,
  pronoun_it,
};

const char *TraitText[] = {
    "Patience",
    "Engagement",
    "Burnout",
    "Labor XP",
    "Pronoun XP",
    "Chair XP",
    "Notetaker XP",
};

enum Trait {
    trait_patience,
    trait_engagement,
    trait_burnout,
    trait_labor_xp,
    trait_pronoun_xp,
    trait_chair_xp,
    trait_notetaker_xp,
    trait_end,
};


struct Person {
    char name[100];
    s32 trait[trait_end];
    Pronoun pronouns[5];
    s32 iu;
};
enum Phase {
    phase_none,
    phase_call_to_order,
    phase_notetaker_choose,
    /*
    phase_notetaker_nominate,
    phase_notetaker_second,
    phase_notetaker_assign,
    */
    phase_take_attendance,
    phase_read_minutes,
    phase_approve_minutes,
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

const char *RatingText[] = {
    "Procedure",
    "Inclusion",
    "Knowledge",
    "Democracy"
};
enum Rating {
    rating_procedure,
    rating_inclusion,
    rating_knowledge,
    rating_democracy,
    rating_end
};
#define people_max 4

// State
struct {
    b32 scroll_down;
    ImGuiTextBuffer log_buffer;
    f32 scale = 2.0;
    b32 initialized;
    Person *selected;
    Person *nominated;
    Person *nominator;
    Person *notetaker;
    Person *chair;
    s32 notetaker_idx = -1;
    Person *people_arr[people_max];
    s32 rating[rating_end];
    Phase phase;
    b32 called_to_order = 0 ;
    b32 read_previous_meeting_notes;
    b32 choose_note_taker;
    char logs[200][200];
    s32 log_idx = 0;
    const char *chair_str = "The chair";
    //const char *notetaker_str = "The notetaker";
} __s;

//
// Helper
//

char* get_role_or_name(Person *person) {
    if (person == __s.chair) return (char *)__s.chair_str;
    //if (person == __s.notetaker) return (char *)__s.notetaker_str;
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

void rating(s32 procedure, s32 democracy = 0){
    __s.rating[rating_procedure] += procedure;
    __s.rating[rating_democracy] += democracy;
    for (s32 idx = 0; idx < ArrayCount(__s.people_arr); ++idx) {
        Person *person = __s.people_arr[idx];
        if(person && __s.rating[rating_procedure] < -person->trait[trait_patience]) {
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

b32 action_basic(const char * title, const char * str) {
    if (imgui::Selectable(title)) {
        __s.log_buffer.clear();
        // The chair calls the meeting to order
        __s.log_buffer.appendf("%s:\n%s\n", get_role_or_name(__s.selected), str);
        rating(1);
        return true;
        rating(-1);
    }
    return false;
}

b32 action_chair(const char * title, const char * str, Phase phase) {
    if (__s.phase == phase-1 && __s.selected == __s.chair) {
        if (imgui::Selectable(title)) {
            __s.log_buffer.clear();
            // The chair calls the meeting to order
            __s.log_buffer.appendf("%s:\n%s\n", get_role_or_name(__s.selected), str);
            rating(1);
            __s.phase = phase;
            return true;
        }
    }
    return false;
}
// I move to..
b32 action_non_chair(const char * title, const char * str, Phase phase) {
    if (__s.phase == phase-1){
        if( __s.selected != __s.chair) {
            if (imgui::Selectable(title)) {
                __s.log_buffer.clear();
                // The chair calls the meeting to order
                __s.log_buffer.appendf("%s:\nI move to %s\n", get_role_or_name(__s.selected), str);
                rating(1);
                __s.phase = phase;
                return true;
            }
        } else if( __s.selected == __s.chair) {
            if (imgui::Selectable(title)) {
                __s.log_buffer.clear();
                // The chair calls the meeting to order
                __s.log_buffer.appendf("%s:\nI will entertain a motion to %s\n", get_role_or_name(__s.selected), str);
                rating(1);
                return true;
            }
        }
    }
    return false;
}

b32 action_nominate(Phase phase) {
    if (__s.phase == phase-1 && __s.selected != __s.chair) {
        for (s32 idx = 0; idx < people_max; ++idx) {
            Person *person2 = __s.people_arr[idx];
            if (person2 != __s.chair && person2 != __s.selected) {
                char label[200];
                snprintf(label, 200, "Nominate %s", person2->name);
                if (imgui::Selectable(label)) {
                    __s.log_buffer.clear();
                    // The chair calls the meeting to order
                    __s.log_buffer.appendf("%s:\nI nominate %s\n", get_role_or_name(__s.selected), person2->name);
                    __s.log_buffer.appendf("%s:\nIs there a second?\n", get_role_or_name(__s.chair));
                    __s.phase = phase;
                    __s.nominated = person2;
                    __s.nominator = __s.selected;
                    printf("selected %s\n", __s.selected->name);
                    printf("nominator %s\n", __s.nominator->name);
                    rating(1, 1);
                    return true;
                }
            }
        }
    }
    return false;
}

b32 second_nomination(Phase phase) {
    b32 show  = __s.phase == phase-1 && 
                __s.selected != __s.nominated && 
                __s.selected != __s.nominator && 
                __s.selected != __s.chair;
    if (show) {
        char label[200];
        snprintf(label, 200, "Second %s's Nomination", __s.nominated->name);
        if (imgui::Selectable(label)) {
            __s.log_buffer.clear();
            // The chair calls the meeting to order
            __s.log_buffer.appendf("%s:\nI second %s's nomination\n", get_role_or_name(__s.selected), __s.nominated->name);
            __s.phase = phase;
            rating(1, 1);
            return true;
        }
    }
    return false;
}
#define person_name_max 100

void create_person(Person * person, const char * name, s32 patience) {
    snprintf(person->name,person_name_max, "%s", name);
    for (s32 idx = 0; idx < trait_end; idx++) {
        person->trait[idx] = 0;
    }
    person->trait[trait_patience] = patience;




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
        __s.people_arr[3] = (Person * )malloc(200);
        create_person(__s.people_arr[0], "James", 10);
        create_person(__s.people_arr[1], "Sam", 8);
        create_person(__s.people_arr[2], "Darius", 5);
        create_person(__s.people_arr[3], "Ian", 3);
        __s.chair = __s.people_arr[0];


    };


    render_begin({0,0}, {screen.x,screen.y}, "Rusty's Rules");
    ImGui::SetWindowFontScale(__s.scale);



    //
    // Draw people
    // 
    
    for (s32 idx = 0; idx < ArrayCount(__s.people_arr); ++idx) {
        Person *person = __s.people_arr[idx];
        if(person) {
            if(imgui::Selectable(person->name, person == __s.selected)){
                __s.selected = person;
                //imgui::OpenPopup("actions-to-take");
            }
            if(__s.people_arr[idx] == __s.chair) {
                imgui::SameLine();
                imgui::Text("(chair)");
            }
            if(__s.people_arr[idx] == __s.notetaker) {
                imgui::SameLine();
                imgui::Text("(notetaker)");
            }
        }
    }

    //
    // Actions
    //

    if (__s.selected) {
        imgui::BeginTabBar("People");
        if(imgui::BeginTabItem("Actions")) {
            action_chair("call to order", "I call this meeting to order at 6:00", phase_call_to_order);
            action_chair("select notetaker", "Now we'll move on to selecting a notetaker.", phase_notetaker_choose);
            action_chair("attendenance", "Now we'll take attendance", phase_take_attendance); 
            action_chair("previous meeting notes", "Now we'll read the previous meeting notes", phase_read_minutes);
            action_chair("approve meeting notes", "Now we'll approve the previous meeting notes", phase_approve_minutes);
            action_chair("approve agenda", "Now we'll approve this meeting's agenda", phase_approve_agenda);
            action_chair("announcements", "Now we'll brief announcements", phase_announcements); 
            action_chair("reports", "Now we'll reports", phase_reports); 
            action_chair("old business", "Now we'll old business", phase_old_business); 
            action_chair("new business", "Now we'll new business", phase_new_business); 
            action_chair("good and welfare", "Now we'll good and welfare", phase_good_and_welfare); 
            action_chair("pick next chair", "Now we'll pick the next chair", phase_chair_choose); 
            action_nominate(phase_chair_nomination);
            second_nomination(phase_chair_second);
            //action_non_chair("critique meeting", "moves to critique meeting", phase_meeting_critique); 
            if(action_non_chair("adjorn", "adjorn", phase_end)) {
                __s.log_buffer.appendf("Meeting complete!");
            }

            action_basic("point of order", "Point of order.");
            action_basic("point of information", "Point of information.");
            action_basic("raise hand", "<Raises hand>");
            imgui::EndTabItem();
        }
        if(imgui::BeginTabItem("Profile")) {
            imgui::BeginTable("Skills", 2);
            for (s32 idx = 0; idx < trait_end; idx++) {
                imgui::TableNextColumn();
                imgui::Text("%s", TraitText[idx]);
                imgui::TableNextColumn();
                imgui::Text("%d", __s.selected->trait[idx]);
            }
            imgui::EndTable();
            imgui::EndTabItem();
        }
        imgui::EndTabBar();
    } else {
        imgui::SeparatorText("Notes");
        ImGui::BeginChild("notes", {screen.x -30,150}, ImGuiChildFlags_Borders);
        if (__s.scroll_down) {
            __s.scroll_down = false;
            imgui::SetScrollFromPosY(10000);
        }
        imgui::TextWrapped("%s", __s.log_buffer.c_str());
        imgui::EndChild();
        imgui::SeparatorText("Meeting Rating");
        imgui::BeginTable("Rating", 2);
        for (s32 idx = 0; idx < rating_end; idx++) {
            imgui::TableNextColumn();
            imgui::Text("%s", RatingText[idx]);
            imgui::TableNextColumn();
            imgui::Text("%d", __s.rating[idx]);
        }
        imgui::EndTable();
    }
    imgui::End();
    
}

