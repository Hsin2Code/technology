#ifndef NEMO_EXTENSION_SIMS_H
#define NEMO_EXTENSION_SIMS_H

#include <glib-object.h>


G_BEGIN_DECLS

/* Declarations for the open terminal extension object.  This object will be
 * instantiated by nemo.  It implements the GInterfaces 
 * exported by libnemo. */

#define NEMO_TYPE_SIMS	  (nemo_sims_get_type ())
#define NEMO_SIMS(o)	  (G_TYPE_CHECK_INSTANCE_CAST ((o), NEMO_TYPE_SIMS, NemoSims))
#define NEMO_IS_SIMS(o)	  (G_TYPE_CHECK_INSTANCE_TYPE ((o), NEMO_TYPE_SIMS))

typedef struct _NemoSims      NemoSims;
typedef struct _NemoSimsClass NemoSimsClass;

struct _NemoSims {
	GObject parent_slot;
};

struct _NemoSimsClass {
	GObjectClass parent_slot;
};

GType nemo_sims_get_type (void);
void  nemo_sims_register_type (GTypeModule *module);

G_END_DECLS

#endif
