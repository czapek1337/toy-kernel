#pragma once

#include "../boot/stivale2.h"

void smp_init(struct stivale2_struct_tag_smp *smp_tag);
void smp_init_cpu(struct stivale2_smp_info *ap_info);
