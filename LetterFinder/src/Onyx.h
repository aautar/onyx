#include <memory>
#include <vector>
#include <map>
#include <CL/opencl.h>

#include "OnyxProgram.h"

class Onyx
{
	protected:
		cl_platform_id*								clPlatformIDs;
		std::map<cl_platform_id, cl_device_id*>		cdDevices;

		std::vector<cl_platform_id>								platformIds;
		std::map<cl_platform_id, std::vector<cl_device_id>>		deviceIds;

		std::vector<std::string>								allPlatformNames;
		std::map<cl_platform_id, std::vector<std::string>>		platformToDeviceNames;

		int getAllPlatformIds()
		{
			cl_uint numPlatforms = 0; 
			cl_int ciErrNum = 0;

			// Get OpenCL platform count
			ciErrNum = clGetPlatformIDs(0, NULL, &numPlatforms);
			if (ciErrNum != CL_SUCCESS) {
				return ERROR_CANT_GET_PLATFORM_IDS;
			}
			else if(numPlatforms == 0) {
				return ERROR_NO_PLATFORM;
			} else {
 
				// there's 1+ platforms, make space for ID's
				clPlatformIDs = (cl_platform_id*)malloc(numPlatforms * sizeof(cl_platform_id));
				if(clPlatformIDs == NULL) {
					return ERROR_NO_MEMORY_FOR_PLATFORM_IDS;
				}

				// get platform info for a platforms (modified call to clGetPlatformIDs)
				ciErrNum = clGetPlatformIDs(numPlatforms, clPlatformIDs, NULL);
				if(ciErrNum != CL_SUCCESS) {
					return ERROR_CANT_GET_PLATFORM_INFO;
				}

				for(cl_uint i=0; i<numPlatforms; i++) {
					platformIds.push_back(clPlatformIDs[i]);
				}

			}

			return CL_SUCCESS;
		}


		int getDeviceIdsForPlatform(cl_platform_id platformId)
		{
			cl_uint			numDevices = 0;

			clGetDeviceIDs(platformId, CL_DEVICE_TYPE_GPU, 0, NULL, &numDevices);
			if(numDevices == 0) {
				return ERROR_NO_DEVICES_FOR_PLATFORM;
			}

			cdDevices[platformId] = (cl_device_id*)malloc(numDevices * sizeof(cl_device_id));
			clGetDeviceIDs(platformId, CL_DEVICE_TYPE_GPU, numDevices, cdDevices[platformId], NULL);

			for(cl_uint i=0; i<numDevices; i++) {
				deviceIds[platformId].push_back(cdDevices[platformId][i]);
			}

			return CL_SUCCESS;
		}

	public:

		static const int ERROR_CANT_GET_PLATFORM_IDS = -1000;
		static const int ERROR_NO_PLATFORM = -2000;
		static const int ERROR_NO_MEMORY_FOR_PLATFORM_IDS = -3000;
		static const int ERROR_CANT_GET_PLATFORM_INFO = -4000;
		static const int ERROR_NO_DEVICES_FOR_PLATFORM = -5000;
		
		Onyx() : clPlatformIDs(NULL) {

			getAllPlatformIds();

			for(size_t i=0; i<platformIds.size(); i++) {

				cl_platform_id pid = platformIds[i];

				getDeviceIdsForPlatform(pid);
				allPlatformNames.push_back( getPlatformName(pid) );

				for(size_t j=0; j<deviceIds[pid].size(); j++) {
					platformToDeviceNames[pid].push_back( getDeviceName(deviceIds[pid][j]) );
				}
			}

		}

		~Onyx() {

			for(size_t i=0; i<platformIds.size(); i++) {
				cl_platform_id pid = platformIds[i];
				free(cdDevices[pid]);
			}

			if(clPlatformIDs != NULL) {
				free(clPlatformIDs);
				clPlatformIDs = NULL;
			}
		}
		
		cl_context createCLContext(cl_device_id deviceId)
		{
			cl_int err;
			cl_context context = clCreateContext(0, 1, &deviceId, NULL, NULL, &err);
			return context;
		}

		cl_device_id getDeviceId(size_t deviceIndex)
		{
			for(size_t i=0; i<platformIds.size(); i++) {
				cl_platform_id pid = platformIds[i];
				for(size_t j=0; j<deviceIds[pid].size(); j++) {
					if(deviceIndex == j) {
						return deviceIds[pid][j];
					}
				}
			}

			return NULL;
		}

		OnyxProgram* createProgram(const std::string& filepath, cl_device_id deviceId, cl_context devContext)
		{
			OnyxProgram* prog = new OnyxProgram(filepath, deviceId, devContext);
			return prog;
		}

		std::string getPlatformName(cl_platform_id& clPlatformId)
		{
			char chBuffer[1024];    
			cl_int ciErrNum = clGetPlatformInfo(clPlatformId, CL_PLATFORM_NAME, 1024, &chBuffer, NULL);
			if(ciErrNum == CL_SUCCESS) {
				return std::string(chBuffer);
			}

			return std::string();
		}

		std::string getDeviceName(cl_device_id& clDeviceId)
		{
			char chBuffer[1024];    
			cl_int ciErrNum = clGetDeviceInfo(clDeviceId, CL_DEVICE_NAME, sizeof(chBuffer), chBuffer, NULL);
			if(ciErrNum == CL_SUCCESS) {
				return std::string(chBuffer);
			}

			return std::string();
		}

};
