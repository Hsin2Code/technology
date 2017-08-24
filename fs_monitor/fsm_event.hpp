#pragma once

enum fsm_event_flag
{
    NoOp = 0,                     /**< No event has occurred. */
    PlatformSpecific = (1 << 0),  /**< Platform-specific placeholder for event type that cannot currently be mapped. */
    Created = (1 << 1),           /**< An object was created. */
    Updated = (1 << 2),           /**< An object was updated. */
    Removed = (1 << 3),           /**< An object was removed. */
    Renamed = (1 << 4),           /**< An object was renamed. */
    OwnerModified = (1 << 5),     /**< The owner of an object was modified. */
    AttributeModified = (1 << 6), /**< The attributes of an object were modified. */
    MovedFrom = (1 << 7),         /**< An object was moved from this location. */
    MovedTo = (1 << 8),           /**< An object was moved to this location. */
    IsFile = (1 << 9),            /**< The object is a file. */
    IsDir = (1 << 10),            /**< The object is a directory. */
    IsSymLink = (1 << 11),        /**< The object is a symbolic link. */
    Link = (1 << 12),             /**< The link count of an object has changed. */
    Overflow = (1 << 13)          /**< The event queue has overflowed. */
};
