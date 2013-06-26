/* Automatically generated nanopb constant definitions */
/* Generated by nanopb-0.2.1 at Sun Jun  9 15:04:56 2013. */

#include "nanim.pb.h"



const pb_field_t im_bci_nanim_Frame_fields[7] = {
    PB_FIELD(  1, STRING  , REQUIRED, CALLBACK, im_bci_nanim_Frame, imageName, imageName, 0),
    PB_FIELD(  2, INT32   , REQUIRED, STATIC, im_bci_nanim_Frame, duration, imageName, 0),
    PB_FIELD(  3, FLOAT   , REQUIRED, STATIC, im_bci_nanim_Frame, u1, duration, 0),
    PB_FIELD(  4, FLOAT   , REQUIRED, STATIC, im_bci_nanim_Frame, v1, u1, 0),
    PB_FIELD(  5, FLOAT   , REQUIRED, STATIC, im_bci_nanim_Frame, u2, v1, 0),
    PB_FIELD(  6, FLOAT   , REQUIRED, STATIC, im_bci_nanim_Frame, v2, u2, 0),
    PB_LAST_FIELD
};

const pb_field_t im_bci_nanim_Animation_fields[3] = {
    PB_FIELD(  1, STRING  , REQUIRED, CALLBACK, im_bci_nanim_Animation, name, name, 0),
    PB_FIELD(  2, MESSAGE , REPEATED, CALLBACK, im_bci_nanim_Animation, frames, name, &im_bci_nanim_Frame_fields),
    PB_LAST_FIELD
};

const pb_field_t im_bci_nanim_Image_fields[6] = {
    PB_FIELD(  1, STRING  , REQUIRED, CALLBACK, im_bci_nanim_Image, name, name, 0),
    PB_FIELD(  2, INT32   , REQUIRED, STATIC, im_bci_nanim_Image, width, name, 0),
    PB_FIELD(  3, INT32   , REQUIRED, STATIC, im_bci_nanim_Image, height, width, 0),
    PB_FIELD(  4, ENUM    , REQUIRED, STATIC, im_bci_nanim_Image, format, height, 0),
    PB_FIELD(  5, BYTES   , REQUIRED, CALLBACK, im_bci_nanim_Image, pixels, format, 0),
    PB_LAST_FIELD
};

const pb_field_t im_bci_nanim_Nanim_fields[5] = {
    PB_FIELD(  1, MESSAGE , REPEATED, CALLBACK, im_bci_nanim_Nanim, images, images, &im_bci_nanim_Image_fields),
    PB_FIELD(  2, MESSAGE , REPEATED, CALLBACK, im_bci_nanim_Nanim, animations, images, &im_bci_nanim_Animation_fields),
    PB_FIELD(  3, STRING  , OPTIONAL, CALLBACK, im_bci_nanim_Nanim, author, animations, 0),
    PB_FIELD(  4, STRING  , OPTIONAL, CALLBACK, im_bci_nanim_Nanim, license, author, 0),
    PB_LAST_FIELD
};


/* Check that field information fits in pb_field_t */
#if !defined(PB_FIELD_16BIT) && !defined(PB_FIELD_32BIT)
STATIC_ASSERT((pb_membersize(im_bci_nanim_Animation, frames) < 256 && pb_membersize(im_bci_nanim_Nanim, images) < 256 && pb_membersize(im_bci_nanim_Nanim, animations) < 256), YOU_MUST_DEFINE_PB_FIELD_16BIT_FOR_MESSAGES_im_bci_nanim_Frame_im_bci_nanim_Animation_im_bci_nanim_Image_im_bci_nanim_Nanim)
#endif

#if !defined(PB_FIELD_32BIT)
STATIC_ASSERT((pb_membersize(im_bci_nanim_Animation, frames) < 65536 && pb_membersize(im_bci_nanim_Nanim, images) < 65536 && pb_membersize(im_bci_nanim_Nanim, animations) < 65536), YOU_MUST_DEFINE_PB_FIELD_32BIT_FOR_MESSAGES_im_bci_nanim_Frame_im_bci_nanim_Animation_im_bci_nanim_Image_im_bci_nanim_Nanim)
#endif
