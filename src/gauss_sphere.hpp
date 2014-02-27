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

        float mAlpha;
        float mBeta;

    public:
        GaussSphere(cl::Context& context, cl::CommandQueue& command_queue, const float alpha=1.f, const float beta=90.f)
            : mContext(context), mCommandQueue(command_queue), mAlpha(alpha), mBeta(beta)
        { 
            mResolution = pow(2*(1+ceil(mBeta/mAlpha))+1, 2);  
            std::cout << "mResolution: " << mResolution << std::endl;
        }

        void setupBuffers(const std::vector<float>& normals)
        {
            mNumNormals = normals.size();
            mNormalsBufSize = sizeof(float) * mNumNormals;
            std::cout<< "Creating buffer with  " << mNumNormals << " normals and " << mResolution << " pixels" << "sizeof: " << sizeof(float) << std::endl;

            mNormalsBuf = cl::Buffer(mContext, CL_MEM_READ_WRITE, mNormalsBufSize);
            mNorthHemisphereBuf = cl::Buffer(mContext, CL_MEM_READ_WRITE, sizeof(cl_int) * mResolution);
            mSouthHemisphereBuf = cl::Buffer(mContext, CL_MEM_READ_WRITE, sizeof(cl_int) * mResolution);

            std::vector<int> val(mResolution, 0);
            mCommandQueue.enqueueWriteBuffer(mNorthHemisphereBuf, CL_TRUE, 0, sizeof(cl_int) * mResolution, val.data());
            mCommandQueue.enqueueWriteBuffer(mSouthHemisphereBuf, CL_TRUE, 0, sizeof(cl_int) * mResolution, val.data());
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
            std::cout << "Read south hemisphere" << std::endl;
            output.clear();
            output.resize(mResolution);
            mCommandQueue.enqueueReadBuffer(mSouthHemisphereBuf, CL_TRUE, 0, sizeof(cl_int) * mResolution, output.data());
        }

        void runKernel(cl::Kernel& kernel)
        {
                kernel.setArg(0, mNormalsBuf );
                kernel.setArg(1, mNorthHemisphereBuf);
                kernel.setArg(2, mSouthHemisphereBuf);
                kernel.setArg(3, mAlpha);
                kernel.setArg(4, mBeta);

                int global = mNumNormals/4;

                std::cout << "Running with NDRange " << global  << std::endl;
                if(mCommandQueue.enqueueNDRangeKernel(kernel, cl::NullRange, cl::NDRange(global), cl::NullRange) != CL_SUCCESS) {
                    throw std::runtime_error("Running kernel failed");
                }

                mCommandQueue.finish();
                std::cout  << "ran" << std::endl;
        }
};
