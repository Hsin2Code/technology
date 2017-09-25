#ifndef _NEMO_SIMS_CALLBACK_H
#define _NEMO_SIMS_CALLBACK_H
#include <libnemo-extension/nemo-extension-types.h>
#include <libnemo-extension/nemo-file-info.h>
#include <libnemo-extension/nemo-menu-provider.h>



void
preset_level_func (NemoMenuItem *item, gpointer provider);

void
preset_inspect_func (NemoMenuItem *item, gpointer provider);

void
preset_revise_func (NemoMenuItem *item, gpointer provider);

void
local_open_func (NemoMenuItem *item, gpointer provider);

void
network_apply_func (NemoMenuItem *item, gpointer provider);

void
network_change_func (NemoMenuItem *item, gpointer provider);

void
network_issue_func (NemoMenuItem *item, gpointer provider);

void
network_decrypt_func (NemoMenuItem *item, gpointer provider);

void
network_declass_func (NemoMenuItem *item, gpointer provider);

void
issue_doc_func (NemoMenuItem *item, gpointer provider);

void
issue_review_func (NemoMenuItem *item, gpointer provider);

typedef void (*_callback)(NemoMenuItem *, gpointer);

static const _callback item_type_func[] = { preset_level_func,
                                            preset_inspect_func,
                                            preset_revise_func,
                                            local_open_func,
                                            network_apply_func,
                                            network_change_func,
                                            network_issue_func,
                                            network_decrypt_func,
                                            network_declass_func,
                                            issue_doc_func,
                                            issue_review_func };

static const char *item_type_str[] = { "Sims::Preset::Level",
                                       "Sims::Preset::Inspect",
                                       "Sims::Preset::Revise",
                                       "Sims::Local::Open",
                                       "Sims::Network::Apply",
                                       "Sims::Network::Change",
                                       "Sims::Network::Issue",
                                       "Sims::Network::Decrpt",
                                       "Sims::Network::Declass",
                                       "Sims::Issue::Doc",
                                       "Sims::Issue::Review" };
enum item_type {
    PRESET_LEVEL,
    PRESET_INSPECT,
    PRESET_REVISE,
    LOCAL_OPEN,
    NETWORK_APPLY,
    NETWORK_CHANGE,
    NETWORK_ISSUE,
    NETWORK_DECRPT,
    NETWORK_DECLASS,
    ISSUE_DOC,
    ISSUE_REVIEW
};

#endif
