#include <iostream>
#include <string>
#include "Onyx.h"


void main()
{
	size_t workgroupSize = 2;
	size_t haystacksSize = 256;
	char* haystack = new char[haystacksSize];
	//int* outputBuffer = new int[haystacksSize];
	strcpy(haystack, "rice is the seed of the grass species Oryza sativa (Asian rice) or Oryza glaberrima (African rice).");

	int* haystackSize = new int[1];
	haystackSize[0] = (int)strlen(haystack);

	Onyx* onyxCL = new Onyx();

	cl_device_id primaryDeviceId = onyxCL->getDeviceId(0);
	cl_context ctx = onyxCL->createCLContext(primaryDeviceId);
	
	OnyxProgram* prog = onyxCL->createProgram("kernel.cl", primaryDeviceId, ctx);
	cl_mem paramA = prog->createInputBuffer(haystack, haystacksSize);
	cl_mem paramB = prog->createInputBuffer(haystackSize, sizeof(int));
	cl_mem output = prog->createOutputBuffer(sizeof(int) * haystacksSize);

	cl_mem kernelArgs[3] = {paramA, paramB, output};
	int* computedResult = (int*)prog->executeKernel("part1", kernelArgs, 3, workgroupSize, output, sizeof(int) * haystacksSize);


	//for(int i=0; i < num; i++) {
	//	std::cout << "x = " << computedResult[i] << "\n";
	//}

	delete [] computedResult;
	delete onyxCL;

	/*
	cl_device_id cldev = deviceIdsForPrimaryPlatform[0];


	
    cl_int err;
    cl_context context = clCreateContext(0, 1, &cldev, NULL, NULL, &err);

    std::string sourceCode = loadFile("kernel.cl");
	const char* codeStr = sourceCode.c_str();
	size_t codeStrLength = strlen(codeStr);

	cl_program prog = clCreateProgramWithSource(context, 1, &codeStr, &codeStrLength, &err);

	err = clBuildProgram(prog, 0, NULL, NULL, NULL, NULL);
	if(err == CL_SUCCESS) {
		cl_build_status buildStatus;
		err = clGetProgramBuildInfo(prog, cldev, CL_PROGRAM_BUILD_STATUS, sizeof(buildStatus), &buildStatus, NULL);

		char *buildLog;
		size_t retValSize;
		err = clGetProgramBuildInfo(prog, cldev, CL_PROGRAM_BUILD_LOG, 0, NULL, &retValSize);

		buildLog = new char[retValSize+1];
		err = clGetProgramBuildInfo(prog, cldev, CL_PROGRAM_BUILD_LOG, retValSize, buildLog, NULL);
		buildLog[retValSize] = '\0';
	}



	int num = 10;
    float *a = new float[num];
    float *b = new float[num];
    for(int i=0; i < num; i++) {
        a[i] = 1.0f * i;
        b[i] = 1.0f * i;
    }

	// program specific
	cl_kernel kernel = clCreateKernel(prog, "part1", &err);

	//our input arrays
	cl_mem cl_a = clCreateBuffer(context, CL_MEM_READ_ONLY|CL_MEM_COPY_HOST_PTR, sizeof(float) * num, a, &err); // copied to GPU for us?!
	cl_mem cl_b = clCreateBuffer(context, CL_MEM_READ_ONLY|CL_MEM_COPY_HOST_PTR, sizeof(float) * num, b, &err); // copied to GPU for us?!
	//our output array
	cl_mem cl_c = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(float) * num, NULL, &err);


    //set the arguements of our kernel
	cl_command_queue commandQueue = clCreateCommandQueue(context, cldev, 0, &err);
    err  = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *) &cl_a);
    err  = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void *) &cl_b);
    err  = clSetKernelArg(kernel, 2, sizeof(cl_mem), (void *) &cl_c);
	clFinish(commandQueue);

	cl_event evt;
	size_t workGroupSize[1];
	workGroupSize[0] = num;
	err = clEnqueueNDRangeKernel(commandQueue, kernel, 1, NULL, workGroupSize, NULL, 0, NULL, &evt);
    clReleaseEvent(evt);


	float c_done[10];
    err = clEnqueueReadBuffer(commandQueue, cl_c, CL_TRUE, 0, sizeof(float) * num, &c_done, 0, NULL, &evt);
    clReleaseEvent(evt);


	for(int i=0; i < num; i++) {
		float x = c_done[i];
	}
	*/



}