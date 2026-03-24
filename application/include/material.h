#ifndef MATERIAL_H
#define MATERIAL_H

#include "cglm/struct/vec3.h"

struct material {
    vec3s ambient;
    vec3s diffuse;
    vec3s specular;
    float shininess;
};

/* http://devernay.free.fr/cours/opengl/materials.html */
enum material_types {
    EMERALD,
    JADE,
    OBSIDIAN,
    PEARL,
    RUBY,
    TURQUOISE,
    BRASS,
    BRONZE,
    CHROME,
    COPPER,
    GOLD,
    SILVER,
    BLACK_PLASTIC,
    CYAN_PLASTIC,
    GREEN_PLASTIC,
    RED_PLASTIC,
    WHITE_PLASTIC,
    YELLOW_PLASTIC,
    BLACK_RUBBER,
    CYAN_RUBBER,
    GREEN_RUBBER,
    RED_RUBBER,
    WHITE_RUBBER,
    YELLOW_RUBBER,
    MATERIAL_TYPES_LENGTH,
};

/* clang-format off */
struct material MATERIALS[MATERIAL_TYPES_LENGTH] = {
    [EMERALD]        = { (vec3s) { 0.0215, 0.1745, 0.0215 },       (vec3s) { 0.07568, 0.61424, 0.07568 },    (vec3s) { 0.633, 0.727811, 0.633 },             0.6        },
    [JADE]           = { (vec3s) { 0.135, 0.2225, 0.1575 },        (vec3s) { 0.54, 0.89, 0.63 },             (vec3s) { 0.316228, 0.316228, 0.316228 },       0.1        },
    [OBSIDIAN]       = { (vec3s) { 0.05375, 0.05, 0.06625 },       (vec3s) { 0.18275, 0.17, 0.22525 },       (vec3s) { 0.332741, 0.328634, 0.346435 },       0.3        },
    [PEARL]          = { (vec3s) { 0.25, 0.20725, 0.20725 },       (vec3s) { 1, 0.829, 0.829 },              (vec3s) { 0.296648, 0.296648, 0.296648 },       0.088      },
    [RUBY]           = { (vec3s) { 0.1745, 0.01175, 0.01175 },     (vec3s) { 0.61424, 0.04136, 0.04136 },    (vec3s) { 0.727811, 0.626959, 0.626959 },       0.6        },
    [TURQUOISE]      = { (vec3s) { 0.1, 0.18725, 0.1745 },         (vec3s) { 0.396, 0.74151, 0.69102 },      (vec3s) { 0.297254, 0.30829, 0.306678 },        0.1        },
    [BRASS]          = { (vec3s) { 0.329412, 0.223529, 0.027451 }, (vec3s) { 0.780392, 0.568627, 0.113725 }, (vec3s) { 0.992157, 0.941176, 0.807843 },       0.21794872 },
    [BRONZE]         = { (vec3s) { 0.2125, 0.1275, 0.054 },        (vec3s) { 0.714, 0.4284, 0.18144 },       (vec3s) { 0.393548, 0.271906, 0.166721 },       0.2        },
    [CHROME]         = { (vec3s) { 0.25, 0.25, 0.25 },             (vec3s) { 0.4, 0.4, 0.4 },                (vec3s) { 0.774597, 0.774597, 0.774597 },       0.6        },
    [COPPER]         = { (vec3s) { 0.19125, 0.0735, 0.0225 },      (vec3s) { 0.7038, 0.27048, 0.0828 },      (vec3s) { 0.256777, 0.137622, 0.086014 },       0.1        },
    [GOLD]           = { (vec3s) { 0.24725, 0.1995, 0.0745 },      (vec3s) { 0.75164, 0.60648, 0.22648 },    (vec3s) { 0.628281, 0.555802, 0.366065 },       0.4        },
    [SILVER]         = { (vec3s) { 0.19225, 0.19225, 0.19225 },    (vec3s) { 0.50754, 0.50754, 0.50754 },    (vec3s) { 0.508273, 0.508273, 0.508273 },       0.4        },
    [BLACK_PLASTIC]  = { (vec3s) { 0.0, 0.0, 0.0 },                (vec3s) { 0.01, 0.01, 0.01 },             (vec3s) { 0.50, 0.50, 0.50 },                   0.25       },
    [CYAN_PLASTIC]   = { (vec3s) { 0.0, 0.1, 0.06 },               (vec3s) { 0.0, 0.50980392, 0.50980392 },  (vec3s) { 0.50196078, 0.50196078, 0.50196078 }, 0.25       },
    [GREEN_PLASTIC]  = { (vec3s) { 0.0, 0.0, 0.0 },                (vec3s) { 0.1, 0.35, 0.1 },               (vec3s) { 0.45, 0.55, 0.45 },                   0.25       },
    [RED_PLASTIC]    = { (vec3s) { 0.0, 0.0, 0.0 },                (vec3s) { 0.5, 0.0, 0.0 },                (vec3s) { 0.7, 0.6, 0.6 },                      0.25       },
    [WHITE_PLASTIC]  = { (vec3s) { 0.0, 0.0, 0.0 },                (vec3s) { 0.55, 0.55, 0.55 },             (vec3s) { 0.70, 0.70, 0.70 },                   0.25       },
    [YELLOW_PLASTIC] = { (vec3s) { 0.0, 0.0, 0.0 },                (vec3s) { 0.5, 0.5, 0.0 },                (vec3s) { 0.60, 0.60, 0.50 },                   0.25       },
    [BLACK_RUBBER]   = { (vec3s) { 0.02, 0.02, 0.02 },             (vec3s) { 0.01, 0.01, 0.01 },             (vec3s) { 0.4, 0.4, 0.4 },                      0.078125   },
    [CYAN_RUBBER]    = { (vec3s) { 0.0, 0.05, 0.05 },              (vec3s) { 0.4, 0.5, 0.5 },                (vec3s) { 0.04, 0.7, 0.7 },                     0.078125   },
    [GREEN_RUBBER]   = { (vec3s) { 0.0, 0.05, 0.0 },               (vec3s) { 0.4, 0.5, 0.4 },                (vec3s) { 0.04, 0.7, 0.04 },                    0.078125   },
    [RED_RUBBER]     = { (vec3s) { 0.05, 0.0, 0.0 },               (vec3s) { 0.5, 0.4, 0.4 },                (vec3s) { 0.7, 0.04, 0.04 },                    0.078125   },
    [WHITE_RUBBER]   = { (vec3s) { 0.05, 0.05, 0.05 },             (vec3s) { 0.5, 0.5, 0.5 },                (vec3s) { 0.7, 0.7, 0.7 },                      0.078125   },
    [YELLOW_RUBBER]  = { (vec3s) { 0.05, 0.05, 0.0 },              (vec3s) { 0.5, 0.5, 0.4 },                (vec3s) { 0.7, 0.7, 0.04 },                     0.078125   },
};
/* clang-format on */

#endif
