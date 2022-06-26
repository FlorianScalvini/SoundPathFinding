//
// Created by Florian SCALVINI on 26/06/22.
//


__global__ void convolution(char * img, float * img_out, const float * __restrict__ mask,
                            const int halfKernel, const int width, const int height)
{
    extern __shared__ float lInput[];


    const unsigned int indexRowG = blockIdx.y * blockDim.y + threadIdx.y;
    const unsigned int indexColG = blockIdx.x * blockDim.x + threadIdx.x;
    const unsigned int localRowLen = 2*halfKernel + blockDim.x;
    const unsigned int idxLocal = halfKernel + threadIdx.x + localRowLen * (threadIdx.y + halfKernel);
    const unsigned int idx = indexColG + width * indexRowG ;

    if(indexColG >= width && indexRowG >= height)
    {
        lInput[idxLocal] = 0;
    }
    else
    {
        lInput[idxLocal] = img[idx];
    }



    __syncthreads();
    int fIndex = 0;
    float sum = 0;
    for (int row = -halfKernel; row <= halfKernel; row+=1)
    {
        int curRow = row * localRowLen;
        for (int col = -halfKernel; col <= halfKernel; col+=1, fIndex++)
        {
            sum += lInput[idxLocal + curRow + col] * mask[fIndex];
        }
    }

    img_out[idx] = sum;
    return;
}