__kernel void part1(__global char* haystack, __global int* haystackSize, __global int* needles)
{
	unsigned hs = haystackSize[0];

    unsigned int partition = get_global_id(0);
	unsigned int start = (hs/2) * partition;
	unsigned int end = start + (hs/2);
    
	for(unsigned int i=start; i<end; i++) {

		needles[i] = -1;

		if(i+3 < end && haystack[i] == 'r' && haystack[i+1] == 'i' && haystack[i+2] == 'c' && haystack[i+3] == 'e') {
			needles[i] = i;
		}
	}
}