#include <memory>
#include <vector>
#include <map>
#include <CL/opencl.h>

class OnyxProgram
{
	protected:
		cl_device_id	deviceId;
		cl_context		devContext;
		cl_program		prog;
		std::string		progFilePath;
		std::string		buildLog;

		std::string loadCLFile(const std::string& filepath)
		{
			std::string filedata;

			char buf[2056];
			FILE* fp = fopen(filepath.c_str(), "r");

			while(true) {
				char* ret = fgets(buf, 2056, fp);
				if(ret == 0) {
					break;
				}

				filedata.append(buf);
			}

			fclose(fp);

			return filedata;
		}

		bool createProgram() 
		{
			cl_int buildOk;
			cl_int err;

			std::string sourceCode = loadCLFile(progFilePath.c_str());
			const char* codeStr = sourceCode.c_str();
			size_t codeStrLength = strlen(codeStr);

			prog = clCreateProgramWithSource(devContext, 1, &codeStr, &codeStrLength, &err);

			buildOk = clBuildProgram(prog, 0, NULL, NULL, NULL, NULL);

			cl_build_status buildStatus;
			err = clGetProgramBuildInfo(prog, deviceId, CL_PROGRAM_BUILD_STATUS, sizeof(buildStatus), &buildStatus, NULL);

			char *log;
			size_t retValSize;
			err = clGetProgramBuildInfo(prog, deviceId, CL_PROGRAM_BUILD_LOG, 0, NULL, &retValSize);

			log = new char[retValSize+1];
			err = clGetProgramBuildInfo(prog, deviceId, CL_PROGRAM_BUILD_LOG, retValSize, log, NULL);
			log[retValSize] = '\0';

			buildLog = log;
			delete[] log;

			if(buildOk == CL_SUCCESS) {
				return true;
			}

			return false;
		}

	public:
		OnyxProgram(const std::string& _filepath, cl_device_id _deviceId, cl_context _devContext) : progFilePath(_filepath), deviceId(_deviceId), devContext(_devContext)  {
			createProgram();
		}

		cl_mem createInputBuffer(void* buf, size_t len)
		{
			cl_int err;
			return clCreateBuffer(devContext, CL_MEM_READ_ONLY|CL_MEM_COPY_HOST_PTR, len, buf, &err); 
		}

		cl_mem createOutputBuffer(size_t len)
		{
			cl_int err;
			return clCreateBuffer(devContext, CL_MEM_WRITE_ONLY, sizeof(unsigned char) * len, NULL, &err);
		}

		void* executeKernel(const std::string& kernelName, cl_mem* kernelArgs, size_t numKernelArgs, size_t workSize, cl_mem outputBuffer, size_t outputBufferSize)
		{
			cl_int err;
			cl_event evt;

			cl_kernel kernel = clCreateKernel(prog, kernelName.c_str(), &err);
			cl_command_queue commandQueue = clCreateCommandQueue(devContext, deviceId, 0, &err);

			for(size_t i=0; i<numKernelArgs; i++) {
				err = clSetKernelArg(kernel, (cl_uint)i, sizeof(cl_mem), (void*) &(kernelArgs[i]));
			}

			size_t workGroupSize[1];
			workGroupSize[0] = workSize;
			err = clEnqueueNDRangeKernel(commandQueue, kernel, 1, NULL, workGroupSize, NULL, 0, NULL, &evt);
			clReleaseEvent(evt);

			char* computedResult = new char[outputBufferSize];
			err = clEnqueueReadBuffer(commandQueue, outputBuffer, CL_TRUE, 0, outputBufferSize, computedResult, 0, NULL, &evt);
			clReleaseEvent(evt);

			return (void*)computedResult;
		}

};