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
    const char name[100];
    s32 iu;
    Pronoun pronouns[5];
    struct {
        s32 pronouns;
        s32 chairing;
        s32 recording_secretary;
        s32 iu;
    } training;
    s32 patience; // minimum procedure
};
enum Phase {
    phase_none,
    phase_,
};

struct {
    b32 initialized;
    Person *selected;
    Person *chair;
    s32 notetaker_idx = -1;
    Person people_arr[10] = {
        { "James", 510, {pronoun_he}},
        { "Sam", 510, {pronoun_she}},
        { "Darian", 510, {pronoun_they}}
    };
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
#define LOG snprintf(__s.logs[__s.log_idx++], 200, 
#define sa_log_text ImGui::LogText
#define sa_debug_log imgui::DebugLog
#define sa imgui

void procedure(s32 change) {
    __s.rating.procedure += change;
    char c[10] ={0};
    if (change > 0) {
        c[0] = '+';
    }
    sa_debug_log("procedure %d (%s%d)\n", __s.rating.procedure, c, change);
}
void reset() {
    __s.selected = 0;
}
/*
void get_who(char * who) {
    snprintf(who, 200, "%s", __s.people_arr[__s.selected_idx].name);
}
*/

char* get_role_or_name(Person *person) {
    if (person == __s.chair)
        return (char *)__s.chair_str;
    return (char *)person->name;
}


b32 basic_action(const char * str, Person * person, b32 *condition) {
    sa_debug_log("%s %s\n", get_role_or_name(__s.selected), str);
    if(__s.selected == person) {
        if (!*condition) {
            procedure(1);
            *condition ^= 1;
        } else {
            sa_debug_log("... AGAIN\n");
            procedure(-1);
        }
    } else {
        sa_debug_log("%s says, uhm, that's my job..\n", get_role_or_name(person));
        // not chair
        procedure(-1);
    }
    return false;
}


void game_loop() {

    // 
    // Initialize
    //
    
    if (!__s.initialized) {
        __s.initialized = 1;
        srand((u32)time(NULL));  
        __s.chair = &__s.people_arr[0];

    } 

    sa_begin({0,0}, {300,600}, "Rusty's Rules");

    //
    // Draw people
    // 
    
    for (s32 idx = 0; idx < ArrayCount(__s.people_arr); ++idx) {
        Person *person = &__s.people_arr[idx];
        if(*person->name) {
            sa::PushID(idx);
            if(sa::RadioButton(person->name, person == __s.selected)){
                __s.selected = person;
            }
            if(&__s.people_arr[idx] == __s.chair) {
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
        if(sa_button("Call To Order")) {
            basic_action("calls the meeting to order", __s.chair, &__s.called_to_order);
        }
        if (sa_button("Move to read previous meeting notes")) {
            basic_action("moves to read the previous meeting notes", __s.chair, 
            &__s.read_previous_meeting_notes);
        }
        if (sa_button("Move to Choose Notetaker")) {
            basic_action("moves to choose notetaker", __s.chair, 
                &__s.choose_note_taker);
        }
        if (sa_button("Nominate Notetaker")) {
            basic_action("nominates someone", __s.chair, &__s.choose_note_taker); 
        }
        if (sa_button("Take Attendance")) {
            basic_action("takes attendance", __s.chair, &__s.choose_note_taker); 
        }
        if (sa_button("Move to New Business")) {
            basic_action("moves to new business", __s.chair, &__s.choose_note_taker); 
        }
        if (sa_button("Move to Old Business")) {
            basic_action("moves to old business", __s.chair, &__s.choose_note_taker); 
        }
        if (sa_button("Call to Adjorn")) {
            basic_action("calls to adjorn", __s.chair, &__s.choose_note_taker); 
        }
    }

    sa_show_debug_log_window({300, -85}, {500,685});
}

