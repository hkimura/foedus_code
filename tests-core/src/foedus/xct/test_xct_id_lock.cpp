/*
 * Copyright (c) 2014, Hewlett-Packard Development Company, LP.
 * The license and distribution terms for this file are placed in LICENSE.txt.
 */
#include <foedus/test_common.hpp>
#include <foedus/epoch.hpp>
#include <foedus/assorted/atomic_fences.hpp>
#include <foedus/xct/xct_id.hpp>
#include <stdint.h>
#include <gtest/gtest.h>
#include <algorithm>
#include <chrono>
#include <thread>
#include <vector>

namespace foedus {
namespace xct {

const int THREADS = 10;
const int KEYS = 100;

XctId keys[KEYS];
std::vector<std::thread> threads;
bool  done[THREADS];

void sleep_enough() {
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
}

void init() {
    for (int i = 0; i < KEYS; ++i) {
        keys[i].data_ = 0;
        EXPECT_FALSE(keys[i].is_valid());
        EXPECT_FALSE(keys[i].is_deleted());
        EXPECT_FALSE(keys[i].is_keylocked());
        EXPECT_FALSE(keys[i].is_latest());
        EXPECT_FALSE(keys[i].is_rangelocked());
    }
    for (int i = 0; i < THREADS; ++i) {
        done[i] = false;
    }
}
void join_all() {
    for (std::thread& th : threads) {
        th.join();
    }
    threads.clear();
}

TEST(XctIdLockTest, NoConflict) {
    init();
    for (int i = 0; i < THREADS; ++i) {
        threads.emplace_back(std::thread([i]{ keys[i].keylock_unconditional(); }));
    }
    join_all();
    assorted::memory_fence_acquire();
    for (int i = 0; i < THREADS; ++i) {
        EXPECT_FALSE(keys[i].is_valid());
        EXPECT_FALSE(keys[i].is_deleted());
        EXPECT_TRUE(keys[i].is_keylocked());
        EXPECT_FALSE(keys[i].is_latest());
        EXPECT_FALSE(keys[i].is_rangelocked());
    }
    for (int i = 0; i < THREADS; ++i) {
        keys[i].release_keylock();
    }
    for (int i = 0; i < THREADS; ++i) {
        EXPECT_EQ(0, keys[i].data_);
    }
}

TEST(XctIdLockTest, Conflict) {
    init();
    for (int i = 0; i < THREADS; ++i) {
        threads.emplace_back(std::thread([i]{
            keys[i / 2].keylock_unconditional(); done[i] = true;
        }));
        sleep_enough();
    }
    assorted::memory_fence_acquire();
    for (int i = 0; i < THREADS; ++i) {
        EXPECT_FALSE(keys[i / 2].is_valid());
        EXPECT_FALSE(keys[i / 2].is_deleted());
        EXPECT_TRUE(keys[i / 2].is_keylocked());
        EXPECT_FALSE(keys[i / 2].is_latest());
        EXPECT_FALSE(keys[i / 2].is_rangelocked());
        if (i % 2 == 0) {
            EXPECT_TRUE(done[i]);
        } else {
            EXPECT_FALSE(done[i]);
        }
    }
    for (int i = 0; i < THREADS / 2; ++i) {
        keys[i].release_keylock();
    }
    sleep_enough();
    for (int i = 0; i < THREADS; ++i) {
        EXPECT_FALSE(keys[i / 2].is_valid());
        EXPECT_FALSE(keys[i / 2].is_deleted());
        EXPECT_TRUE(keys[i / 2].is_keylocked());
        EXPECT_FALSE(keys[i / 2].is_latest());
        EXPECT_FALSE(keys[i / 2].is_rangelocked());
        EXPECT_TRUE(done[i]);
    }
    join_all();
    for (int i = 0; i < THREADS / 2; ++i) {
        keys[i].release_keylock();
        EXPECT_EQ(0, keys[i].data_);
    }
}

}  // namespace xct
}  // namespace foedus
