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
        int mNormalsBufSize;
        cl::Buffer mNorthHemisphereBuf;
        cl::Buffer mSouthHemisphereBuf;
        int mResolution;

    public:
        GaussSphere(cl::Context& context, cl::CommandQueue& command_queue)
            : mContext(context), mCommandQueue(command_queue)
        { }

        void setupBuffers(const std::vector<glm::vec4>& normals, int w, int h)
        {
            mNumNormals = normals.size();
            mResolution = w * h;
            mNormalsBufSize = sizeof(cl_float) * 4 * mNumNormals;
            std::cout<< "Creating buffer with  " << mNumNormals << " normals and " << mResolution << " pixels" << std::endl;

            mNormalsBuf = cl::Buffer(mContext, CL_MEM_READ_WRITE, mNormalsBufSize);
            mNorthHemisphereBuf = cl::Buffer(mContext, CL_MEM_READ_WRITE, sizeof(cl_int) * mResolution);
            mSouthHemisphereBuf = cl::Buffer(mContext, CL_MEM_READ_WRITE, sizeof(cl_int) * mResolution);

            if(mCommandQueue.enqueueWriteBuffer(mNormalsBuf, CL_TRUE, 0, mNormalsBufSize, normals.data()) != CL_SUCCESS) {
                throw std::runtime_error("GaussSphere::Failed to upload normals");
            }
        }

        void readNorthHemisphere(std::vector<cl_int>& output)
        {
            std::cout << "Read north hemisphere" << std::endl;
            output.clear();
            output.resize(mResolution);
            mCommandQueue.enqueueReadBuffer(mNorthHemisphereBuf, CL_TRUE, 0, sizeof(cl_int) * mResolution, output.data());
            mCommandQueue.finish();
        }

        void readSouthHemisphere(std::vector<cl_int>& output)
        {
            output.clear();
            output.resize(mResolution);
            mCommandQueue.enqueueReadBuffer(mSouthHemisphereBuf, CL_TRUE, 0, sizeof(cl_int) * mResolution, output.data());
        }

        void readNormals(std::vector<glm::vec4>& output)
        {
            std::cout << "readNormals" << std::endl;
            output.clear();
            output.resize(mNumNormals);
            mCommandQueue.enqueueReadBuffer(mNormalsBuf, CL_TRUE, 0, sizeof(glm::vec4) * mNumNormals, output.data());
            mCommandQueue.finish();
        }

        void runKernel(cl::Kernel& kernel)
        {
                kernel.setArg(0, mNormalsBuf );
                kernel.setArg(1, mNorthHemisphereBuf);
                kernel.setArg(2, mSouthHemisphereBuf);

                int global = mNumNormals;

                std::cout << "Running with NDRange " << global  << std::endl;
                if(mCommandQueue.enqueueNDRangeKernel(kernel, cl::NullRange, cl::NDRange(global), cl::NullRange) != CL_SUCCESS) {
                    throw std::runtime_error("Running kernel failed");
                }

                mCommandQueue.finish();
                std::cout  << "ran" << std::endl;
        }
};
