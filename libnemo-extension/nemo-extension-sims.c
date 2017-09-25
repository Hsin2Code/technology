#ifdef HAVE_CONFIG_H
 #include <config.h> /* for GETTEXT_PACKAGE */
#endif

#include "nemo-extension-sims.h"
#include "nemo-sims-callback.h"


#include <libnemo-extension/nemo-extension-types.h>
#include <libnemo-extension/nemo-file-info.h>
#include <libnemo-extension/nemo-menu-provider.h>
#include <glib.h>
#include <gio/gio.h>
#include <gtk/gtk.h>

#include <syslog.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h> /* for atoi */
#include <string.h> /* for strcmp */
#include <unistd.h> /* for chdir */
#include <sys/stat.h>


#define ICONS_PATH "/usr/lib/vrv/icons/"


static void nemo_sims_instance_init (NemoSims      *cvs);
static void nemo_sims_class_init    (NemoSimsClass *class);

static GType sims_type = 0;

static GObjectClass *object_class = NULL;

/* 支持的文件格式 */
static struct {
    char *mime_type;
    gboolean is_added;
} archive_mime_types[] = {
    { "", TRUE },
    { "", TRUE },
    { "", TRUE },
    { NULL, FALSE },
};

static gboolean
unsupported_scheme (NemoFileInfo *file)
{
    return TRUE;
    int i = 0;
    for (i = 0; archive_mime_types[i].mime_type != NULL; i++) {
        if (nemo_file_info_is_mime_type (file, archive_mime_types[i].mime_type)) {
            return TRUE;
		}
    }
    return FALSE;
}

#define MENU_APPEND_ITEM(type, label, tip, icon)                        \
    {                                                                   \
        NemoMenuItem *item = nemo_menu_item_new (item_type_str[type], label, tip, icon); \
        nemo_menu_append_item (menu, item);                             \
        g_object_set_data_full (G_OBJECT (item),                        \
                                "files",                                \
                                nemo_file_info_list_copy (files),       \
                                (GDestroyNotify) nemo_file_info_list_free); \
        g_signal_connect (item,                                         \
                          "activate",                                   \
                          G_CALLBACK (item_type_func[type]),            \
                          provider);                                    \
    }

static GList *
nemo_sims_get_file_items (NemoMenuProvider *provider,
                          GtkWidget *window,
                          GList *files)
{
	if (g_list_length (files) != 1) /* 选定多个文件不显示 */
        return NULL;
    /* if (nemo_file_info_is_mime_type ((NemoFileInfo *)files->data, "text")) */
    /*     return NULL; */

    GList *items = NULL;
    /* 预定密 菜单 */
    {
        NemoMenu *menu = nemo_menu_new();
        NemoMenuItem *item = nemo_menu_item_new("Sims::Preset",
                                                "预定密",
                                                "预定级菜单",
                                                ICONS_PATH"Preset.bmp");
        nemo_menu_item_set_submenu(item, menu);

        MENU_APPEND_ITEM(PRESET_LEVEL, "预定密", "文件预定密", ICONS_PATH"PresetLevel.bmp");
        MENU_APPEND_ITEM(PRESET_INSPECT, "查看密级", "查看文件密级", ICONS_PATH"PresetInspect.bmp");
        MENU_APPEND_ITEM(PRESET_REVISE, "修改密级", "修改文件密级", ICONS_PATH"PresetRevise.bmp");

        items = g_list_append (items, item);
    }
    /* 密级标志管理系统子菜单项目 */
    {
        NemoMenu *menu = nemo_menu_new();
        NemoMenuItem *item = nemo_menu_item_new("Sims",
                                                "密级标志管理系统",
                                                "密级标志管理",
                                                ICONS_PATH"Sims.bmp");
        nemo_menu_item_set_submenu(item, menu);
        /* -------------- 本地流程 --------------- */
        MENU_APPEND_ITEM(LOCAL_OPEN, "安全打开", "安全打开文件", ICONS_PATH"LocalOpen.bmp");
        /* -------------- 网络流程 --------------- */
        MENU_APPEND_ITEM(NETWORK_APPLY, "申请定密", "申请文件定密", ICONS_PATH"NetworkApply.bmp");
        MENU_APPEND_ITEM(NETWORK_CHANGE, "密级变更", "变更文件密级", ICONS_PATH"NetworkChange.bmp");
        MENU_APPEND_ITEM(NETWORK_ISSUE, "签发", "签发文件", ICONS_PATH"NetworkIssue.bmp");
        MENU_APPEND_ITEM(NETWORK_DECRPT, "申请解密", "申请文件解密", ICONS_PATH"NetworkDecrypt.bmp");
        MENU_APPEND_ITEM(NETWORK_DECLASS, "申请脱密", "申请文件脱密", ICONS_PATH"NetworkDeclass.bmp");
        /* -------------- 外发管理 --------------- */
        MENU_APPEND_ITEM(ISSUE_DOC, "外发文档", "外发文档", ICONS_PATH"IssueDoc.bmp");
        MENU_APPEND_ITEM(ISSUE_REVIEW, "外发授权", "外发授权", ICONS_PATH"IssueReview.bmp");

        g_list_append(items, item);
    }
	return items;
}

static void
nemo_sims_menu_provider_iface_init (NemoMenuProviderIface *iface)
{
	// 右键点击桌面的， 目前不需要
    // iface->get_background_items = nemo_sims_get_background_items;
	iface->get_file_items = nemo_sims_get_file_items;
}

static void
nemo_sims_instance_init (NemoSims *cvs)
{

}

static void
nemo_sims_class_init (NemoSimsClass *class)
{
    g_assert (object_class == NULL);
    object_class = g_type_class_peek_parent (class);
}

static void
nemo_sims_class_finalize (NemoSimsClass *class)
{
	g_assert (object_class != NULL);
	g_object_unref (object_class);
	object_class = NULL;
}

GType
nemo_sims_get_type (void)
{
	return sims_type;
}

void
nemo_sims_register_type (GTypeModule *module)
{
	static const GTypeInfo info = {
		sizeof (NemoSimsClass),
		(GBaseInitFunc) NULL,
		(GBaseFinalizeFunc) NULL,
		(GClassInitFunc) nemo_sims_class_init,
		(GClassFinalizeFunc) nemo_sims_class_finalize,
		NULL,
		sizeof (NemoSims),
		0,
		(GInstanceInitFunc) nemo_sims_instance_init,
	};

	static const GInterfaceInfo menu_provider_iface_info = {
		(GInterfaceInitFunc) nemo_sims_menu_provider_iface_init,
		NULL,
		NULL
	};

	sims_type = g_type_module_register_type (module,
                                             G_TYPE_OBJECT,
                                             "Sims",
                                             &info, 0);

	g_type_module_add_interface (module,
                                 sims_type,
                                 NEMO_TYPE_MENU_PROVIDER,
                                 &menu_provider_iface_info);
}
