#include "data.h"

_swtchsg( seg)
int seg;

 /* The EM definition demands that pseudo instructions are aligned
  * at word boundaries.
  */
{	
	cur_seg = seg;
	align_word();
}
