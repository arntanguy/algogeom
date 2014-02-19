#include <CL/cl.hpp>
#include <glm/glm.hpp>

#include <iostream>
#include <stdexcept>

class GaussSphere
{
    private:
        cl::Context mContext;
        cl::CommandQueue mCommandQueue;
        cl::Buffer mNormalsBuf;
        int mNumNormals;
        cl::Buffer mNorthHemisphereBuf;
        cl::Buffer mSouthHemisphereBuf;
        int mResolution;

    public:
        GaussSphere(cl::Context& context, cl::CommandQueue& command_queue)
            : mContext(context), mCommandQueue(command_queue)
        { }

        void setupBuffers(const std::vector<cl_float>& normals, int w, int h)
        {
            mNumNormals = normals.size();
            mResolution = w * h;
            std::cout<< "Creating buffer with  " << mNumNormals << " normals and " << mResolution << " pixels" << std::endl;

            mNormalsBuf = cl::Buffer(mContext, CL_MEM_READ_WRITE, sizeof(cl_float) * mNumNormals);
            mNorthHemisphereBuf = cl::Buffer(mContext, CL_MEM_READ_WRITE, sizeof(cl_int) * mResolution);
            mSouthHemisphereBuf = cl::Buffer(mContext, CL_MEM_READ_WRITE, sizeof(cl_int) * mResolution);

            if(mCommandQueue.enqueueWriteBuffer(mNormalsBuf, CL_TRUE, 0, sizeof(cl_float) * mNumNormals, normals.data()) != CL_SUCCESS) throw "fuck";
        }

        void readNorthHemisphere(std::vector<cl_int>& output)
        {
            output.reserve(mResolution*3);
            mCommandQueue.enqueueReadBuffer(mNormalsBuf, CL_TRUE, 0, sizeof(cl_int) * mResolution, output.data());
        }

        void readSouthHemisphere(std::vector<cl_int>& output)
        {
            output.reserve(mResolution*3);
            mCommandQueue.enqueueReadBuffer(mNormalsBuf, CL_TRUE, 0, sizeof(cl_int) * mResolution, output.data());
        }

        void readNormals(std::vector<cl_float>& output)
        {
            std::cout << "readNormals" << std::endl;
            output.clear();
            output.resize(mNumNormals);
            mCommandQueue.enqueueReadBuffer(mNormalsBuf, CL_TRUE, 0, sizeof(cl_float) * mNumNormals, &output[0]);
            mCommandQueue.finish();
        }

        void runKernel(cl::Kernel& kernel)
        {
                kernel.setArg(0, mNormalsBuf );
                kernel.setArg(1, mNorthHemisphereBuf);
                kernel.setArg(2, mSouthHemisphereBuf);

                int global = mNumNormals / 4;

                std::cout << "Running with NDRange " << global  << std::endl;
                if(mCommandQueue.enqueueNDRangeKernel(kernel, cl::NullRange, cl::NDRange(global), cl::NullRange) != CL_SUCCESS) {
                    throw std::runtime_error("Running kernel failed");
                }

                mCommandQueue.finish();
                std::cout  << "ran" << std::endl;
        }
};
