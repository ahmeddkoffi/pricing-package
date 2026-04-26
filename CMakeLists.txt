cmake_minimum_required(VERSION 4.1)
project(Je_termine_projet_C__)

set(CMAKE_CXX_STANDARD 20)

include_directories(include)

add_executable(test_q11
        src/main.cpp
        src/DataLoader.cpp
        src/ImpliedVolGrid.cpp
        src/BilinearInterpolator.cpp
        src/ImpliedVolSurface.cpp
)

add_executable(test_q12
        src/test_question_1_2.cpp
        src/DataLoader.cpp
        src/ImpliedVolGrid.cpp
        src/BilinearInterpolator.cpp
        src/ImpliedVolSurface.cpp
        src/LocalVolSurface.cpp
)

add_executable(test_q13
        src/test_question_1_3.cpp
        src/DataLoader.cpp
        src/ImpliedVolGrid.cpp
        src/BilinearInterpolator.cpp
        src/ImpliedVolSurface.cpp
        src/BlackScholesPricer.cpp
        src/ArbitrageChecker.cpp
)

add_executable(test_option_a
        src/test_option_a.cpp
        src/NormalRng.cpp
        src/CorrelatedRng.cpp
        src/CIRProcess.cpp
        src/HestonProcess.cpp
        src/EulerSchemePricer.cpp
        src/TruncatedGaussianSchemePricer.cpp
        src/QuadraticExponentialSchemePricer.cpp
        src/BroadieKayaPricer.cpp
        src/HestonSemiAnalyticalPricer.cpp
)
