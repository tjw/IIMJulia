//
//  Header.h
//  IIMJulia
//
//  Created by Timothy Wood on 3/18/12.
//  Copyright (c) 2012 Cocoatoa. All rights reserved.
//

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

