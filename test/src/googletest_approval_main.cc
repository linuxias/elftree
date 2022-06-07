/*
 * Copyright (c) 2020 Samsung Electronics Co., Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#define APPROVALS_GOOGLETEST // This tells Approval Tests to provide a main() - only do this in one cpp file
#include "ApprovalTests.hpp"

#ifdef ARCH_AARCH64
auto directoryDisposer =
    ApprovalTests::Approvals::useApprovalsSubdirectory("approval_tests_aarch64");
#elif ARCH_ARMV7L
auto directoryDisposer =
    ApprovalTests::Approvals::useApprovalsSubdirectory("approval_tests_armv7l");
#elif ARCH_X86_64
auto directoryDisposer =
    ApprovalTests::Approvals::useApprovalsSubdirectory("approval_tests_x86_64");
#elif ARCH_I586
auto directoryDisposer =
    ApprovalTests::Approvals::useApprovalsSubdirectory("approval_tests_i586");
#endif

