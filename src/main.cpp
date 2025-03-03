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

struct {
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
#define sa_debug_log imgui::DebugLog
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
    sa_debug_log("procedure %d (%s%d)\n", __s.rating.procedure, c, change);

    for (s32 idx = 0; idx < ArrayCount(__s.people_arr); ++idx) {
        Person *person = __s.people_arr[idx];
        if(person && __s.rating.procedure < person->tolerance) {
            if (total_people() > 1) {
                sa_debug_log("%s gets up and storms out\n", get_role_or_name(person));
            } else {
                sa_debug_log("%s, defeated, leaves dejectedly\n", get_role_or_name(person));
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
    sa_debug_log("%s %s\n", get_role_or_name(__s.selected), str);
    if(__s.selected == person) {
        if (__s.phase == phase-1) {
            procedure(1);
            __s.phase = phase;
        } else if (__s.phase > phase) {
            sa_debug_log("... AGAIN\n");
            procedure(-1);
        } else {
            sa_debug_log("... but it's not time for that yet.\n");
            procedure(-1);
        }
    } else {
        if (person) {
            sa_debug_log("%s says, uhm, that's my job..\n", get_role_or_name(person));
        }
        // not chair
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

        // seth so dumb
        __s.people_arr[0] = (Person * )malloc(200);
        __s.people_arr[1] = (Person * )malloc(200);
        __s.people_arr[2] = (Person * )malloc(200);
        create_person(__s.people_arr[0], "James", -10);
        create_person(__s.people_arr[1], "Sam", -8);
        create_person(__s.people_arr[2], "Darius", -5);
        __s.chair = __s.people_arr[0];
    };


    sa_begin({0,0}, {300,600}, "Rusty's Rules");

    //
    // Draw people
    // 
    
    for (s32 idx = 0; idx < ArrayCount(__s.people_arr); ++idx) {
        Person *person = __s.people_arr[idx];
        if(person) {
            sa::PushID(idx);
            if(sa::RadioButton(person->name, person == __s.selected)){
                __s.selected = person;
            }
            if(__s.people_arr[idx] == __s.chair) {
                sa::SameLine();
                sa_text("(chair)");
            }
            sa::PopID();
        }
    }



    //
    // Actions
    //

    if (__s.selected) {
        sa::SeparatorText( __s.selected->name);
        if(sa_button("call the meeting to order")) {
            basic_action("calls the meeting to order", __s.chair, phase_call_to_order);
        }
        if (sa_button("Move to read previous meeting notes")) {
            basic_action("moves to read the previous meeting notes", __s.chair, 
                    phase_read_minutes);
        }
        if (sa_button("Move to Choose Notetaker")) {
            basic_action("moves to choose notetaker", __s.chair, 
                phase_choose_notetaker);
        }
        if (sa_button("Take Attendance")) {
            basic_action("takes attendance", __s.chair, phase_take_attendance); 
        }
        if (sa_button("Move to New Business")) {
            basic_action("moves to new business", __s.chair, phase_new_business); 
        }
        if (sa_button("Move to Old Business")) {
            basic_action("moves to old business", __s.chair, phase_old_business); 
        }
        if (sa_button("Call to Adjorn")) {
            basic_action("calls to adjorn", __s.chair, phase_end); 
        }
    }

    sa_show_debug_log_window({300, -85}, {520,705});
}

