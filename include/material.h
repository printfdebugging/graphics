#ifndef MATERIAL_H
#define MATERIAL_H

#include "cglm/struct.h"

struct material {
        vec3s ambient;
        vec3s diffuse;
        vec3s specular;
        float shininess;
};

/* http://devernay.free.fr/cours/opengl/materials.html */
enum material_type {
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
/* NOTE: the website says that we should multiply the shininess by 128f, don't forget that! */
struct material MATERIALS[MATERIAL_TYPES_LENGTH] = {
        [EMERALD]        = { (vec3s) {{ 0.0215f, 0.1745f, 0.0215f }},       (vec3s) {{ 0.07568f, 0.61424f, 0.07568f }},    (vec3s) {{ 0.633f, 0.727811f, 0.633f }},             0.6f        },
        [JADE]           = { (vec3s) {{ 0.135f, 0.2225f, 0.1575f }},        (vec3s) {{ 0.54f, 0.89f, 0.63f }},             (vec3s) {{ 0.316228f, 0.316228f, 0.316228f }},       0.1f       },
        [OBSIDIAN]       = { (vec3s) {{ 0.05375f, 0.05f, 0.06625f }},       (vec3s) {{ 0.18275f, 0.17f, 0.22525f }},       (vec3s) {{ 0.332741f, 0.328634f, 0.346435f }},       0.3f        },
        [PEARL]          = { (vec3s) {{ 0.25f, 0.20725f, 0.20725f }},       (vec3s) {{ 1.0f, 0.829f, 0.829f }},              (vec3s) {{ 0.296648f, 0.296648f, 0.296648f }},     0.088f    },
        [RUBY]           = { (vec3s) {{ 0.1745f, 0.01175f, 0.01175f }},     (vec3s) {{ 0.61424f, 0.04136f, 0.04136f }},    (vec3s) {{ 0.727811f, 0.626959f, 0.626959f }},       0.6f        },
        [TURQUOISE]      = { (vec3s) {{ 0.1f, 0.18725f, 0.1745f }},         (vec3s) {{ 0.396f, 0.74151f, 0.69102f }},      (vec3s) {{ 0.297254f, 0.30829f, 0.306678f }},        0.1f        },
        [BRASS]          = { (vec3s) {{ 0.329412f, 0.223529f, 0.027451f }}, (vec3s) {{ 0.780392f, 0.568627f, 0.113725f }}, (vec3s) {{ 0.992157f, 0.941176f, 0.807843f }},       0.21794872f },
        [BRONZE]         = { (vec3s) {{ 0.2125f, 0.1275f, 0.054f }},        (vec3s) {{ 0.714f, 0.4284f, 0.18144f }},       (vec3s) {{ 0.393548f, 0.271906f, 0.166721f }},       0.2f        },
        [CHROME]         = { (vec3s) {{ 0.25f, 0.25f, 0.25f }},             (vec3s) {{ 0.4f, 0.4f, 0.4f }},                (vec3s) {{ 0.774597f, 0.774597f, 0.774597f }},       0.6f        },
        [COPPER]         = { (vec3s) {{ 0.19125f, 0.0735f, 0.0225f }},      (vec3s) {{ 0.7038f, 0.27048f, 0.0828f }},      (vec3s) {{ 0.256777f, 0.137622f, 0.086014f }},       0.1f        },
        [GOLD]           = { (vec3s) {{ 0.24725f, 0.1995f, 0.0745f }},      (vec3s) {{ 0.75164f, 0.60648f, 0.22648f }},    (vec3s) {{ 0.628281f, 0.555802f, 0.366065f }},       0.4f        },
        [SILVER]         = { (vec3s) {{ 0.19225f, 0.19225f, 0.19225f }},    (vec3s) {{ 0.50754f, 0.50754f, 0.50754f }},    (vec3s) {{ 0.508273f, 0.508273f, 0.508273f }},       0.4f        },
        [BLACK_PLASTIC]  = { (vec3s) {{ 0.0f, 0.0f, 0.0f }},                (vec3s) {{ 0.01f, 0.01f, 0.01f }},             (vec3s) {{ 0.50f, 0.50f, 0.50f }},                   0.25f       },
        [CYAN_PLASTIC]   = { (vec3s) {{ 0.0f, 0.1f, 0.06f }},               (vec3s) {{ 0.0f, 0.50980392f, 0.50980392f }},  (vec3s) {{ 0.50196078f, 0.50196078f, 0.50196078f }}, 0.25f       },
        [GREEN_PLASTIC]  = { (vec3s) {{ 0.0f, 0.0f, 0.0f }},                (vec3s) {{ 0.1f, 0.35f, 0.1f }},               (vec3s) {{ 0.45f, 0.55f, 0.45f }},                   0.25f       },
        [RED_PLASTIC]    = { (vec3s) {{ 0.0f, 0.0f, 0.0f }},                (vec3s) {{ 0.5f, 0.0f, 0.0f }},                (vec3s) {{ 0.7f, 0.6f, 0.6f }},                      0.25f       },
        [WHITE_PLASTIC]  = { (vec3s) {{ 0.0f, 0.0f, 0.0f }},                (vec3s) {{ 0.55f, 0.55f, 0.55f }},             (vec3s) {{ 0.70f, 0.70f, 0.70f }},                   0.25f       },
        [YELLOW_PLASTIC] = { (vec3s) {{ 0.0f, 0.0f, 0.0f }},                (vec3s) {{ 0.5f, 0.5f, 0.0f }},                (vec3s) {{ 0.60f, 0.60f, 0.50f }},                   0.25f       },
        [BLACK_RUBBER]   = { (vec3s) {{ 0.02f, 0.02f, 0.02f }},             (vec3s) {{ 0.01f, 0.01f, 0.01f }},             (vec3s) {{ 0.4f, 0.4f, 0.4f }},                      0.078125f   },
        [CYAN_RUBBER]    = { (vec3s) {{ 0.0f, 0.05f, 0.05f }},              (vec3s) {{ 0.4f, 0.5f, 0.5f }},                (vec3s) {{ 0.04f, 0.7f, 0.7f }},                     0.078125f   },
        [GREEN_RUBBER]   = { (vec3s) {{ 0.0f, 0.05f, 0.0f }},               (vec3s) {{ 0.4f, 0.5f, 0.4f }},                (vec3s) {{ 0.04f, 0.7f, 0.04f }},                    0.078125f   },
        [RED_RUBBER]     = { (vec3s) {{ 0.05f, 0.0f, 0.0f }},               (vec3s) {{ 0.5f, 0.4f, 0.4f }},                (vec3s) {{ 0.7f, 0.04f, 0.04f }},                    0.078125f   },
        [WHITE_RUBBER]   = { (vec3s) {{ 0.05f, 0.05f, 0.05f }},             (vec3s) {{ 0.5f, 0.5f, 0.5f }},                (vec3s) {{ 0.7f, 0.7f, 0.7f }},                      0.078125f   },
        [YELLOW_RUBBER]  = { (vec3s) {{ 0.05f, 0.05f, 0.0f }},              (vec3s) {{ 0.5f, 0.5f, 0.4f }},                (vec3s) {{ 0.7f, 0.7f, 0.04f }},                     0.078125f   },
};
/* clang-format on */

#endif
