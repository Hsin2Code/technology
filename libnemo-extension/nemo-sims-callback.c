#include <libnemo-extension/nemo-extension-types.h>
#include <libnemo-extension/nemo-file-info.h>
#include <libnemo-extension/nemo-menu-provider.h>

#include <syslog.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "nemo-sims-callback.h"

#define EXEC_PATH "/usr/lib/vrv/vSIMS_Main "

#define EXEC_COMMAND(type)                                              \
    {                                                                   \
        GList *files = g_object_get_data (G_OBJECT (item), "files");    \
        NemoFileInfo *file = files->data;                               \
        char *uri = nemo_file_info_get_uri (file);                      \
        pid_t pid = fork();                                             \
        if (pid == 0) {                                                 \
            setsid();                                                   \
            char temp[256] = {0};                                       \
            snprintf(temp, 255, "%s %s %s", EXEC_PATH, item_type_str[type], uri); \
            syslog(LOG_DEBUG, temp);                                    \
            system(temp);                                               \
            exit(0);                                                    \
        }                                                               \
    }

void
preset_level_func (NemoMenuItem *item, gpointer provider)
{
    EXEC_COMMAND(PRESET_LEVEL);
}

void
preset_inspect_func (NemoMenuItem *item, gpointer provider)
{
    EXEC_COMMAND(PRESET_INSPECT);
}

void
preset_revise_func (NemoMenuItem *item, gpointer provider)
{
    EXEC_COMMAND(PRESET_REVISE);
}
void
local_open_func (NemoMenuItem *item, gpointer provider)
{
    EXEC_COMMAND(LOCAL_OPEN);
}

void
network_apply_func (NemoMenuItem *item, gpointer provider)
{
    EXEC_COMMAND(NETWORK_APPLY);
}

void
network_change_func (NemoMenuItem *item, gpointer provider)
{
    EXEC_COMMAND(NETWORK_CHANGE);
}

void
network_issue_func (NemoMenuItem *item, gpointer provider)
{
    EXEC_COMMAND(NETWORK_ISSUE);
}

void
network_decrypt_func (NemoMenuItem *item, gpointer provider)
{
    EXEC_COMMAND(NETWORK_DECRPT);
}

void
network_declass_func (NemoMenuItem *item, gpointer provider)
{
    EXEC_COMMAND(NETWORK_DECLASS);
}

void
issue_doc_func (NemoMenuItem *item, gpointer provider)
{
    EXEC_COMMAND(ISSUE_DOC);
}

void
issue_review_func (NemoMenuItem *item, gpointer provider)
{
    EXEC_COMMAND(ISSUE_REVIEW);
}
