//
//  julia_miim.cpp
//  IIMJulia
//
//  Created by Timothy Wood on 3/18/12.
//  Copyright (c) 2012 Cocoatoa. All rights reserved.
//

#import "julia.hpp"

#import <stdio.h>
#import <dispatch/dispatch.h>
#import <CoreFoundation/CFDate.h>

extern "C" {
#import <OmniFoundation/OFRandom.h>
}

typedef struct {
    Complex u;
    Complex::Component derivative;
} JuliaEntry;

class JuliaEntryBatch {
public:
    
    unsigned long entryCount;
    JuliaEntry *entries;
    
    inline JuliaEntryBatch(unsigned long count) {
        entryCount = count;
        entries = (typeof(entries))malloc(count * sizeof(*entries));
    }
    
    ~JuliaEntryBatch(void) {
        if (entries)
            free(entries);
    }
    
private:
    JuliaEntryBatch(const JuliaEntryBatch &); // No implicit copies
};

static Complex _iim_julia_preimage(Complex u, Complex c)
{
    return (u - c).sqrt();
}

static Complex::Component MaxDerivative = 1e6;

static void process_batch(dispatch_queue_t batchQueue, JuliaEntryBatch *inBatch, Complex c,
                          dispatch_semaphore_t finishSemaphore, dispatch_queue_t finishQueue, void (^finish_batch)(JuliaEntryBatch *b))
{
    //fprintf(stderr, "processing %ld\n", inBatch->entryCount);
    
    unsigned long rangeStart = 0;
    
    while (rangeStart < inBatch->entryCount) {
        // Break up the input batch to make sure we feed the concurrent queue
        unsigned long rangeEnd = MIN(rangeStart + 0x20000, inBatch->entryCount);
        
        // Up to two entries, one for each of the two possible preimages
        JuliaEntryBatch *outBatch = new JuliaEntryBatch(2*(rangeEnd - rangeStart));
        unsigned long outEntryIndex = 0;
        
        for (unsigned long inEntryIndex = rangeStart; inEntryIndex < rangeEnd; inEntryIndex++) {
            const JuliaEntry *inEntry = &inBatch->entries[inEntryIndex];
            
            Complex u = _iim_julia_preimage(inEntry->u, c);
            Complex::Component derivative = 2*inEntry->derivative*u.length();
            
            //fprintf(stderr, "u: %f, %f d: %f\n", u.r, u.i, derivative);
            
            if (derivative < MaxDerivative) {
                JuliaEntry *outEntry = &outBatch->entries[outEntryIndex];
                
                outEntry[0].u = u;
                outEntry[0].derivative = derivative;            
                outEntry[1].u = -u;
                outEntry[1].derivative = derivative;
                
                outEntryIndex += 2;
            }
        }
        
        rangeStart = rangeEnd;
        
        outBatch->entryCount = outEntryIndex;
        
        dispatch_async(batchQueue, ^{
            process_batch(batchQueue, outBatch, c, finishSemaphore, finishQueue, finish_batch);
        });
    }

    dispatch_semaphore_wait(finishSemaphore, DISPATCH_TIME_FOREVER);
    dispatch_async(finishQueue, ^{
        finish_batch(inBatch);
        dispatch_semaphore_signal(finishSemaphore);
    });
}
                          
void julia_miim(Complex c, Extent xExtent, Extent::Component yCenter, unsigned long width, unsigned long height, julia_result result)
{
    assert(width > 0);
    assert(height > 0);
    assert(result);
    
    // Make the y-axis scale to the x-axis
    Extent::Component yLength = (xExtent.length() / width) * height;
    Extent yExtent(yCenter - yLength/2, yCenter + yLength/2);
    double xStep = width / xExtent.length();
    double yStep = height / yExtent.length();

    Histogram *histogram = new Histogram(width, height);

    dispatch_queue_t resultQueue = dispatch_get_current_queue();
    dispatch_retain(resultQueue);
    
    result = [result copy];
    
    unsigned long maximumFinishingBatchCount = (3*[[NSProcessInfo processInfo] activeProcessorCount]);
    dispatch_semaphore_t finishSemaphore = dispatch_semaphore_create(maximumFinishingBatchCount); // Only allow a certain number of blocks waiting to be finished
    
    dispatch_queue_t finishQueue = dispatch_queue_create("com.cocoatota.julia_miim.finish_batch", DISPATCH_QUEUE_SERIAL);
    
    dispatch_queue_t batchQueue = dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_LOW, 0);
    
    // Initial batch with one random starting point.
    JuliaEntryBatch *batch = new JuliaEntryBatch(1);
    batch->entries[0].u = Complex(4 * OFRandomNextDouble() - 2, 4 * OFRandomNextDouble() - 2);
    batch->entries[0].derivative = 1;
    
    __block CFAbsoluteTime lastNotifyTime = CFAbsoluteTimeGetCurrent();

    void (^finish_batch)(JuliaEntryBatch *batch) = [^(JuliaEntryBatch *batch){
        fprintf(stderr, "finishing %ld\n", batch->entryCount);
        for (unsigned long entryIndex = 0; entryIndex < batch->entryCount; entryIndex++) {
            Complex u = batch->entries[entryIndex].u;

            if (!xExtent.contains(u.r) || !yExtent.contains(u.i)) {
                //fprintf(stderr, "    outside\n");
                return;
            }
            
            unsigned column = floor(xStep * (u.r - xExtent.min()));
            unsigned row = floor(yStep * (u.i - yExtent.min()));
            
            //fprintf(stderr, "    incr %u, %u\n", column, row);
            histogram->increment(column, row);
        }
        delete batch;
        
        // TODO: For very short queues, we might not flush to the result queue (if the whole operation take too little time). Need to work on the totally-done checks.
        CFAbsoluteTime currentTime = CFAbsoluteTimeGetCurrent();
        if (currentTime - lastNotifyTime > 1) {
            fprintf(stderr, "sending partial results\n");
            lastNotifyTime = currentTime;
            const Histogram *resultHistogram = new Histogram(histogram);
            dispatch_async(resultQueue, ^{
                result(resultHistogram);
                delete resultHistogram;
            });
        }
    } copy];
    
    dispatch_async(batchQueue, ^{
        process_batch(batchQueue, batch, c, finishSemaphore, finishQueue, finish_batch);
    });
}
