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

template <typename T> class BulkQueue {
public:
    typedef struct _bucket {
        struct _bucket *next;
        unsigned long entryCount;
        T *entries;
    } bucket;
    
    BulkQueue(void) : _head(NULL), _tail(NULL) {
        
    }
    
    ~BulkQueue(void) {
        bucket *b = _head;
        while (b) {
            bucket *next = b->next;
            done(b);
            b = next;
        }
    }
    
    inline bucket *dequeue(void) {
        assert(_head); // should have enqueued something
        
        bucket *b = _head;
        _head = _head->next;
        if (_head == NULL) {
            assert(b == _tail);
            _tail = NULL; // empty now
        }
        
        return b;
    }
    
    inline bucket *enqueue(unsigned long max_entries) {
        bucket *b = (typeof(b))malloc(sizeof(*b));
        b->next = NULL;
        b->entryCount = 0;
        b->entries = (T *)malloc(sizeof(T) * max_entries);
        
        if (_tail) {
            assert(_head != NULL); // not first bucket
            assert(_tail->next == NULL); // not first bucket
            _tail->next = b;
        } else {
            assert(_head == NULL); // first bucket
            _head = b;
            _tail = b;
        }

        return b;
    }
    
    inline void done(const bucket *b) {
        free(b->entries);
        free((bucket *)b);
    }
    
    inline unsigned long totalCount(void) const {
        unsigned long totalCount = 0;
        
        const bucket *b = _head;
        while (b) {
            totalCount += b->entryCount;
            b = b->next;
        }
        return totalCount;
    }
    
private:
    bucket *_head;
    bucket *_tail;
};

typedef struct {
    Complex u;
    Complex::Component derivative;
} JuliaEntry;

typedef BulkQueue<JuliaEntry> JuliaQueue;

static Complex _iim_julia_preimage(Complex u, Complex c)
{
    return (u - c).sqrt();
}

static void plot_point(Complex u, Extent xExtent, Extent yExtent, double xStep, double yStep, Histogram *histogram)
{
    if (!xExtent.contains(u.r) || !yExtent.contains(u.i)) {
        //fprintf(stderr, "    outside\n");
        return;
    }
    
    unsigned column = floor(xStep * (u.r - xExtent.min()));
    unsigned row = floor(yStep * (u.i - yExtent.min()));
    
    //fprintf(stderr, "    incr %u, %u\n", column, row);
    histogram->increment(column, row);
}

void julia_miim(Complex c, Extent xExtent, Extent::Component yCenter, unsigned long width, unsigned long height, julia_result result)
{
    assert(width > 0);
    assert(height > 0);
    assert(result);
    
    dispatch_queue_t resultQueue = dispatch_get_current_queue();
    dispatch_retain(resultQueue);
    
    result = [result copy];
    
    dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{
        // Make the y-axis scale to the x-axis
        Extent::Component yLength = (xExtent.length() / width) * height;
        Extent yExtent(yCenter - yLength/2, yCenter + yLength/2);
        double xStep = width / xExtent.length();
        double yStep = height / yExtent.length();
       
        CFAbsoluteTime lastNotifyTime = CFAbsoluteTimeGetCurrent();
        Histogram *histogram = new Histogram(width, height);
        
        // Set up a queue with an initial bucket with one random starting point.
        JuliaQueue queue;
        {
            JuliaQueue::bucket *b = queue.enqueue(1);
            b->entries[0].u = Complex(4 * OFRandomNextDouble() - 2, 4 * OFRandomNextDouble() - 2);
            b->entries[0].derivative = 1;
            b->entryCount = 1;
        }
        
        Complex::Component MaxDerivative = 1e6;
        
        while (YES) {
            // Dequeue one bucket and process it.
            const JuliaQueue::bucket *toProcess = queue.dequeue();
            if (toProcess->entryCount == 0) {
                queue.done(toProcess);
                break;
            }
            
            JuliaQueue::bucket *toEnqueue = queue.enqueue(toProcess->entryCount * 2); // Up to two entries, one for each of the two possible preimages
            
            unsigned long outEntryIndex = 0;
            for (unsigned long inEntryIndex = 0; inEntryIndex < toProcess->entryCount; inEntryIndex++) {
                const JuliaEntry *inEntry = &toProcess->entries[inEntryIndex];
                
                Complex u = _iim_julia_preimage(inEntry->u, c);
                Complex::Component derivative = 2*inEntry->derivative*u.length();
                
                //fprintf(stderr, "u: %f, %f d: %f\n", u.r, u.i, derivative);
                
                if (derivative < MaxDerivative) {
                    JuliaEntry *outEntry = &toEnqueue->entries[outEntryIndex];
                    
                    outEntry[0].u = u;
                    outEntry[0].derivative = derivative;
                    plot_point(u, xExtent, yExtent, xStep, yStep, histogram);
                    
                    outEntry[1].u = -u;
                    outEntry[1].derivative = derivative;
                    plot_point(-u, xExtent, yExtent, xStep, yStep, histogram);

                    outEntryIndex += 2;
                }
            }
            
            toEnqueue->entryCount = outEntryIndex;
            queue.done(toProcess);
            
            fprintf(stderr, "totalCount = %ld\n", queue.totalCount());
            
            CFAbsoluteTime currentTime = CFAbsoluteTimeGetCurrent();
            if (currentTime - lastNotifyTime > 1) {
                lastNotifyTime = currentTime;
                const Histogram *resultHistogram = new Histogram(histogram);
                dispatch_async(resultQueue, ^{
                    result(resultHistogram);
                    delete resultHistogram;
                });
            }
        }
        
        dispatch_async(resultQueue, ^{
            result(histogram);
            delete histogram;
        });
    });
}
