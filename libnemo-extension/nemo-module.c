#ifdef HAVE_CONFIG_H
 #include <config.h>
#endif
#include "nemo-extension-sims.h"
#include <libintl.h>

static GType type_list[1];

void
nemo_module_initialize (GTypeModule *module)
{
	g_print ("Initializing nemo-open-terminal extension\n");

	nemo_sims_register_type (module);
	type_list[0] = NEMO_TYPE_SIMS;

	//bindtextdomain (GETTEXT_PACKAGE, GNOMELOCALEDIR);
	//bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");
    //gconf_client_add_dir(gconf_client_get_default(), "/desktop/gnome/lockdown", 0, NULL);
}

void
nemo_module_shutdown (void)
{
	g_print ("Shutting down nemo-open-terminal extension\n");
}

void
nemo_module_list_types (const GType **types,
			    int          *num_types)
{
	*types = type_list;
	*num_types = G_N_ELEMENTS (type_list);
}
