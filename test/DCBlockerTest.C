/* Copyright 2000-2025 Matt Flax <flatmax@flatmax.org>
   This file is part of GTK+ IOStream class set

   GTK+ IOStream is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   GTK+ IOStream is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You have received a copy of the GNU General Public License
   along with GTK+ IOStream
*/
#include "../include/DSP/DCBlocker.H"
#include <Eigen/Dense>
#include <iostream>

int main() {
    DCBlocker blocker;
    int testCount = 0;
    int passedCount = 0;
    int N = 10240;
    
    // Test 1: Remove constant offset
    {
        testCount++;
        Eigen::Matrix<double, Eigen::Dynamic, 2> input(N, 2);
        input.setConstant(1.5);
        
        std::cout << "\nTest 1 Input (first 5 rows):\n" << input.topRows(5) << "\n";
        
        Eigen::Matrix<double, Eigen::Dynamic, 2> output(N, 2);
        Eigen::Matrix<double, Eigen::Dynamic, 2> dcBypassValues(N, 2);
        // Process each sample individually
        for (int i = 0; i < N; i++) {
            blocker.process(input.row(i), output.row(i));
            dcBypassValues.row(i) = blocker.dcBypass;
        }
        
        std::cout << "Test 1 Output:\n" << output << "\n";
        std::cout << "Test 1 DC Bypass Values:\n" << dcBypassValues << "\n";
        
        bool passed = true;
        for (int i = 1; i < N; i++) { // test from the second row as the first row will not DC block
            for (int j = 0; j < 2; j++) {
                double diff = std::abs(output(i,j));
                // std::cout << "Checking output[" << i << "," << j << "]: " << output(i,j) 
                        //   << " (diff: " << diff << ")\n";
                if (diff > 1e-6) {
                    passed = false;
                    // std::cout << "FAIL: Output value exceeds threshold\n";
                    break;
                }
            }
        
            if (passed && std::abs(blocker.dcBypass(0,0) - 1.5) < 1e-6 && 
                         std::abs(blocker.dcBypass(0,1) - 1.5) < 1e-6) {
                passedCount++;
                // std::cout << "Test 1: Remove constant offset - PASSED\n";
            } else {
                // std::cout << "Test 1: Remove constant offset - FAILED\n";
            }
        }
    }    
    // Test 2: Preserve AC signal
    {
        testCount++;

        Eigen::Matrix<double, Eigen::Dynamic, 2> input(N, 2);
        for (int i = 0; i < N; i++) {
            input(i, 0) = input(i, 1) = (i % 2) ? -1.0 : 1.0;
        }
        std::cout << "\nTest 2 Input (first 5 rows):\n" << input.topRows(5) << "\n";
        
        Eigen::Matrix<double, Eigen::Dynamic, 2> output(N, 2);
        Eigen::Matrix<double, Eigen::Dynamic, 2> dcBypassValues(N, 2);
        // Process each sample individually
        for (int i = 0; i < N; i++) {
            blocker.process(input.row(i), output.row(i));
            dcBypassValues.row(i) = blocker.dcBypass;
        }
        
        std::cout << "Test 2 Output:\n" << output << "\n";
        std::cout << "Test 2 DC Bypass Values:\n" << dcBypassValues << "\n";
        
        bool passed = true;
        for (int i = 2; i < N; i++) { // test from the second row as the first row will not DC block
            for (int j = 0; j < 2; j++) {
                double diff = std::abs(output(i,j) - input(i,j));
                // std::cout << "Checking output[" << i << "," << j << "]: " << output(i,j)
                        //   << " vs input: " << input(i,j) << " (diff: " << diff << ")\n";
                if (diff > 1e-6) {
                    passed = false;
                    // std::cout << "FAIL: Output differs from input by more than threshold\n";
                    break;
                }
            }
        
            if (passed && std::abs(blocker.dcBypass(0,0)) < 1e-6 && 
                         std::abs(blocker.dcBypass(0,1)) < 1e-6) {
                passedCount++;
                // std::cout << "Test 2: Preserve AC signal - PASSED\n";
            } else {
                // std::cout << "Test 2: Preserve AC signal - FAILED\n";
            }
        }
    }
    // Test 3: Remove constant offset while preserving AC signal
    {
        testCount++;

        Eigen::Matrix<double, Eigen::Dynamic, 2> input(N, 2);
        for (int i = 0; i < N; i++) {
            input(i, 0) = input(i, 1) = (i % 2) ? 1.5 : 2.5;
        }
        std::cout << "\nTest 3 Input (first 5 rows):\n" << input.topRows(5) << "\n";
        
        Eigen::Matrix<double, Eigen::Dynamic, 2> output(N, 2);
        Eigen::Matrix<double, Eigen::Dynamic, 2> dcBypassValues(N, 2);
        // Process each sample individually
        for (int i = 0; i < N; i++) {
            blocker.process(input.row(i), output.row(i));
            dcBypassValues.row(i) = blocker.dcBypass;
        }
        
        std::cout << "Test 3 Output:\n" << output << "\n";
        std::cout << "Test 3 DC Bypass Values:\n" << dcBypassValues << "\n";
        
        bool passed = true;
        // Expected output after DC blocking should be alternating between -0.5 and 0.5
        Eigen::Matrix<double, Eigen::Dynamic, 2> expected(N, 2);
        for (int i = 0; i < N; i++) {
            expected(i, 0) = expected(i, 1) = (i % 2) ? -0.5 : 0.5;
        }
        expected.row(0).setZero();
        
        for (int i = 1; i < N; i++) { // test from the second row
            for (int j = 0; j < 2; j++) {
                double diff = std::abs(output(i,j) - expected(i,j));
                // std::cout << "Checking output[" << i << "," << j << "]: " << output(i,j)
                        //   << " vs expected: " << expected(i,j) << " (diff: " << diff << ")\n";
                if (diff > 1e-6) {
                    passed = false;
                    // std::cout << "FAIL: Output differs from expected by more than threshold\n";
                    break;
                }
            }
        }
        
        if (passed && std::abs(blocker.dcBypass(0,0) - 2.0) < 1e-6 && 
                     std::abs(blocker.dcBypass(0,1) - 2.0) < 1e-6) {
            passedCount++;
            // std::cout << "Test 3: Remove constant offset while preserving AC signal - PASSED\n";
        } else {
            // std::cout << "Test 3: Remove constant offset while preserving AC signal - FAILED\n";
        }
    }
    
    std::cout << "\nTest results: " << passedCount << "/" << testCount << " passed\n";
    return passedCount == testCount ? 0 : 1;
}
