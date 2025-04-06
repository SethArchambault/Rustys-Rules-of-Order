#ifndef RUSTY_H
#define RUSTY_H

#ifdef ALE_TESTING
#include "sokol/sokol_app.h"
#include "sokol/sokol_gfx.h"
#include "sa_types.h"
#include "imgui/imgui.h"
#include "sa_render.h"
#endif

#define person_name_max 100
#define people_max 4

struct {
    const char * title = "Rusty's Rules";
    V2f32 screen = { 380, 650 };
} project_config;

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

// State
struct {
    b32 scroll_down;
    ImGuiTextBuffer log_buffer;
    V2f32 screen;
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

#endif
