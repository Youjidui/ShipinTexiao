#pragma once

#define RECTWIDTH(rc) (rc.right-rc.left)
#define RECTHEIGHT(rc) (rc.bottom-rc.top)
#define Step_Pixel (0.5f / 255.0f)
#define CEIL(x) (int(x) + ((float(x) - int(x)) > Step_Pixel ? 1 : 0))

#define ROUND(x) (fabs(ceil(fabs(x)) - fabs(x)) < 5e-4 ? (x > 0 ? ceil(x) : floor(x)) : (x) )

