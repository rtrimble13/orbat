#include "orbat/optimizer/constraint.hpp"

#include <cmath>

#include <gtest/gtest.h>

using orbat::core::Vector;
using orbat::optimizer::BoxConstraint;
using orbat::optimizer::Constraint;
using orbat::optimizer::ConstraintSet;
using orbat::optimizer::FullyInvestedConstraint;
using orbat::optimizer::LongOnlyConstraint;

// ========================================================================
// FullyInvestedConstraint Tests
// ========================================================================

TEST(FullyInvestedConstraintTest, DefaultConstructor) {
    FullyInvestedConstraint constraint;
    EXPECT_EQ(constraint.getName(), "FullyInvested");
    EXPECT_FALSE(constraint.getDescription().empty());
}

TEST(FullyInvestedConstraintTest, CustomTolerance) {
    FullyInvestedConstraint constraint(1e-4);
    EXPECT_DOUBLE_EQ(constraint.getTolerance(), 1e-4);
}

TEST(FullyInvestedConstraintTest, NegativeTolerance) {
    EXPECT_THROW(FullyInvestedConstraint(-0.1), std::invalid_argument);
}

TEST(FullyInvestedConstraintTest, SumToOne) {
    FullyInvestedConstraint constraint;
    Vector weights({0.3, 0.4, 0.3});
    EXPECT_TRUE(constraint.isFeasible(weights));
}

TEST(FullyInvestedConstraintTest, SumToOneWithinTolerance) {
    FullyInvestedConstraint constraint(1e-6);
    Vector weights({0.333333, 0.333333, 0.333334});
    EXPECT_TRUE(constraint.isFeasible(weights));
}

TEST(FullyInvestedConstraintTest, SumNotOne) {
    FullyInvestedConstraint constraint;
    Vector weights({0.3, 0.3, 0.3});
    EXPECT_FALSE(constraint.isFeasible(weights));
}

TEST(FullyInvestedConstraintTest, SumGreaterThanOne) {
    FullyInvestedConstraint constraint;
    Vector weights({0.4, 0.4, 0.3});
    EXPECT_FALSE(constraint.isFeasible(weights));
}

TEST(FullyInvestedConstraintTest, EmptyWeights) {
    FullyInvestedConstraint constraint;
    Vector weights;
    EXPECT_FALSE(constraint.isFeasible(weights));
}

TEST(FullyInvestedConstraintTest, SingleAsset) {
    FullyInvestedConstraint constraint;
    Vector weights({1.0});
    EXPECT_TRUE(constraint.isFeasible(weights));
}

TEST(FullyInvestedConstraintTest, LargePortfolio) {
    FullyInvestedConstraint constraint;
    Vector weights(100, 0.01);  // 100 assets, each with 1% weight
    EXPECT_TRUE(constraint.isFeasible(weights));
}

TEST(FullyInvestedConstraintTest, WithShortPositions) {
    FullyInvestedConstraint constraint;
    Vector weights({0.6, -0.2, 0.6});  // Sum = 1.0, but includes short
    EXPECT_TRUE(constraint.isFeasible(weights));
}

// ========================================================================
// LongOnlyConstraint Tests
// ========================================================================

TEST(LongOnlyConstraintTest, DefaultConstructor) {
    LongOnlyConstraint constraint;
    EXPECT_EQ(constraint.getName(), "LongOnly");
    EXPECT_FALSE(constraint.getDescription().empty());
}

TEST(LongOnlyConstraintTest, CustomTolerance) {
    LongOnlyConstraint constraint(1e-4);
    EXPECT_DOUBLE_EQ(constraint.getTolerance(), 1e-4);
}

TEST(LongOnlyConstraintTest, NegativeTolerance) {
    EXPECT_THROW(LongOnlyConstraint(-0.1), std::invalid_argument);
}

TEST(LongOnlyConstraintTest, AllPositive) {
    LongOnlyConstraint constraint;
    Vector weights({0.3, 0.4, 0.3});
    EXPECT_TRUE(constraint.isFeasible(weights));
}

TEST(LongOnlyConstraintTest, WithZero) {
    LongOnlyConstraint constraint;
    Vector weights({0.5, 0.0, 0.5});
    EXPECT_TRUE(constraint.isFeasible(weights));
}

TEST(LongOnlyConstraintTest, WithNegative) {
    LongOnlyConstraint constraint;
    Vector weights({0.6, -0.2, 0.6});
    EXPECT_FALSE(constraint.isFeasible(weights));
}

TEST(LongOnlyConstraintTest, AllNegative) {
    LongOnlyConstraint constraint;
    Vector weights({-0.3, -0.4, -0.3});
    EXPECT_FALSE(constraint.isFeasible(weights));
}

TEST(LongOnlyConstraintTest, WithinNegativeTolerance) {
    LongOnlyConstraint constraint(1e-6);
    Vector weights({0.5, -1e-7, 0.5});  // Slightly negative but within tolerance
    EXPECT_TRUE(constraint.isFeasible(weights));
}

TEST(LongOnlyConstraintTest, EmptyWeights) {
    LongOnlyConstraint constraint;
    Vector weights;
    EXPECT_FALSE(constraint.isFeasible(weights));
}

TEST(LongOnlyConstraintTest, SingleAsset) {
    LongOnlyConstraint constraint;
    Vector weights({1.0});
    EXPECT_TRUE(constraint.isFeasible(weights));
}

TEST(LongOnlyConstraintTest, LargePortfolio) {
    LongOnlyConstraint constraint;
    Vector weights(100, 0.01);
    EXPECT_TRUE(constraint.isFeasible(weights));
}

// ========================================================================
// BoxConstraint Tests
// ========================================================================

TEST(BoxConstraintTest, UniformBoundsConstructor) {
    BoxConstraint constraint(0.1, 0.4);
    EXPECT_EQ(constraint.getName(), "BoxConstraint");
    EXPECT_TRUE(constraint.hasUniformBounds());
    EXPECT_DOUBLE_EQ(constraint.getUniformLower(), 0.1);
    EXPECT_DOUBLE_EQ(constraint.getUniformUpper(), 0.4);
}

TEST(BoxConstraintTest, UniformBoundsInvalid) {
    EXPECT_THROW(BoxConstraint(0.5, 0.2), std::invalid_argument);
}

TEST(BoxConstraintTest, UniformBoundsWithinRange) {
    BoxConstraint constraint(0.1, 0.4);
    Vector weights({0.3, 0.35, 0.35});
    EXPECT_TRUE(constraint.isFeasible(weights));
}

TEST(BoxConstraintTest, UniformBoundsAtLowerBound) {
    BoxConstraint constraint(0.1, 0.4);
    Vector weights({0.1, 0.4, 0.3});
    EXPECT_TRUE(constraint.isFeasible(weights));
}

TEST(BoxConstraintTest, UniformBoundsAtUpperBound) {
    BoxConstraint constraint(0.1, 0.4);
    Vector weights({0.4, 0.4, 0.2});
    EXPECT_TRUE(constraint.isFeasible(weights));
}

TEST(BoxConstraintTest, UniformBoundsBelowLower) {
    BoxConstraint constraint(0.1, 0.4);
    Vector weights({0.05, 0.45, 0.5});
    EXPECT_FALSE(constraint.isFeasible(weights));
}

TEST(BoxConstraintTest, UniformBoundsAboveUpper) {
    BoxConstraint constraint(0.1, 0.4);
    Vector weights({0.3, 0.5, 0.2});
    EXPECT_FALSE(constraint.isFeasible(weights));
}

TEST(BoxConstraintTest, UniformBoundsZeroLower) {
    BoxConstraint constraint(0.0, 0.5);
    Vector weights({0.0, 0.5, 0.5});
    EXPECT_TRUE(constraint.isFeasible(weights));
}

TEST(BoxConstraintTest, PerAssetBoundsConstructor) {
    std::vector<double> lower = {0.0, 0.1, 0.2};
    std::vector<double> upper = {0.5, 0.4, 0.6};
    BoxConstraint constraint(lower, upper);
    EXPECT_FALSE(constraint.hasUniformBounds());
    EXPECT_EQ(constraint.getLowerBounds().size(), 3);
    EXPECT_EQ(constraint.getUpperBounds().size(), 3);
}

TEST(BoxConstraintTest, PerAssetBoundsWithinRange) {
    std::vector<double> lower = {0.0, 0.1, 0.2};
    std::vector<double> upper = {0.5, 0.4, 0.6};
    BoxConstraint constraint(lower, upper);
    Vector weights({0.3, 0.3, 0.4});
    EXPECT_TRUE(constraint.isFeasible(weights));
}

TEST(BoxConstraintTest, PerAssetBoundsAtBounds) {
    std::vector<double> lower = {0.0, 0.1, 0.2};
    std::vector<double> upper = {0.5, 0.4, 0.6};
    BoxConstraint constraint(lower, upper);
    Vector weights({0.0, 0.4, 0.6});
    EXPECT_TRUE(constraint.isFeasible(weights));
}

TEST(BoxConstraintTest, PerAssetBoundsViolation) {
    std::vector<double> lower = {0.0, 0.1, 0.2};
    std::vector<double> upper = {0.5, 0.4, 0.6};
    BoxConstraint constraint(lower, upper);
    Vector weights({0.3, 0.05, 0.65});  // Second asset below lower, third above upper
    EXPECT_FALSE(constraint.isFeasible(weights));
}

TEST(BoxConstraintTest, PerAssetBoundsSizeMismatch) {
    std::vector<double> lower = {0.0, 0.1};
    std::vector<double> upper = {0.5, 0.4, 0.6};
    EXPECT_THROW(BoxConstraint(lower, upper), std::invalid_argument);
}

TEST(BoxConstraintTest, PerAssetBoundsEmpty) {
    std::vector<double> lower;
    std::vector<double> upper;
    EXPECT_THROW(BoxConstraint(lower, upper), std::invalid_argument);
}

TEST(BoxConstraintTest, PerAssetBoundsInvalidRange) {
    std::vector<double> lower = {0.0, 0.5, 0.2};
    std::vector<double> upper = {0.5, 0.4, 0.6};  // Second asset: lower > upper
    EXPECT_THROW(BoxConstraint(lower, upper), std::invalid_argument);
}

TEST(BoxConstraintTest, PerAssetBoundsSizeMismatchWithWeights) {
    std::vector<double> lower = {0.0, 0.1};
    std::vector<double> upper = {0.5, 0.4};
    BoxConstraint constraint(lower, upper);
    Vector weights({0.3, 0.3, 0.4});  // 3 assets, but bounds for 2
    EXPECT_FALSE(constraint.isFeasible(weights));
}

TEST(BoxConstraintTest, CustomTolerance) {
    BoxConstraint constraint(0.1, 0.4, 1e-4);
    EXPECT_DOUBLE_EQ(constraint.getTolerance(), 1e-4);
}

TEST(BoxConstraintTest, NegativeTolerance) {
    EXPECT_THROW(BoxConstraint(0.1, 0.4, -0.1), std::invalid_argument);
}

TEST(BoxConstraintTest, EmptyWeights) {
    BoxConstraint constraint(0.0, 0.5);
    Vector weights;
    EXPECT_FALSE(constraint.isFeasible(weights));
}

// ========================================================================
// ConstraintSet Tests
// ========================================================================

TEST(ConstraintSetTest, DefaultConstructor) {
    ConstraintSet set;
    EXPECT_TRUE(set.empty());
    EXPECT_EQ(set.size(), 0);
}

TEST(ConstraintSetTest, AddConstraint) {
    ConstraintSet set;
    set.add(std::make_shared<FullyInvestedConstraint>());
    EXPECT_EQ(set.size(), 1);
    EXPECT_FALSE(set.empty());
}

TEST(ConstraintSetTest, AddMultipleConstraints) {
    ConstraintSet set;
    set.add(std::make_shared<FullyInvestedConstraint>());
    set.add(std::make_shared<LongOnlyConstraint>());
    set.add(std::make_shared<BoxConstraint>(0.0, 0.4));
    EXPECT_EQ(set.size(), 3);
}

TEST(ConstraintSetTest, AddNullConstraint) {
    ConstraintSet set;
    EXPECT_THROW(set.add(nullptr), std::invalid_argument);
}

TEST(ConstraintSetTest, Clear) {
    ConstraintSet set;
    set.add(std::make_shared<FullyInvestedConstraint>());
    set.add(std::make_shared<LongOnlyConstraint>());
    set.clear();
    EXPECT_TRUE(set.empty());
    EXPECT_EQ(set.size(), 0);
}

TEST(ConstraintSetTest, IsFeasibleSingleConstraint) {
    ConstraintSet set;
    set.add(std::make_shared<FullyInvestedConstraint>());
    Vector weights({0.3, 0.4, 0.3});
    EXPECT_TRUE(set.isFeasible(weights));
}

TEST(ConstraintSetTest, IsFeasibleMultipleConstraints) {
    ConstraintSet set;
    set.add(std::make_shared<FullyInvestedConstraint>());
    set.add(std::make_shared<LongOnlyConstraint>());
    set.add(std::make_shared<BoxConstraint>(0.0, 0.4));
    Vector weights({0.3, 0.35, 0.35});
    EXPECT_TRUE(set.isFeasible(weights));
}

TEST(ConstraintSetTest, IsFeasibleViolatesOne) {
    ConstraintSet set;
    set.add(std::make_shared<FullyInvestedConstraint>());
    set.add(std::make_shared<LongOnlyConstraint>());
    Vector weights({0.6, -0.1, 0.5});  // Violates long-only
    EXPECT_FALSE(set.isFeasible(weights));
}

TEST(ConstraintSetTest, IsFeasibleEmpty) {
    ConstraintSet set;
    Vector weights({0.3, 0.4, 0.3});
    EXPECT_TRUE(set.isFeasible(weights));  // No constraints = always feasible
}

TEST(ConstraintSetTest, GetConstraints) {
    ConstraintSet set;
    auto c1 = std::make_shared<FullyInvestedConstraint>();
    auto c2 = std::make_shared<LongOnlyConstraint>();
    set.add(c1);
    set.add(c2);
    const auto& constraints = set.getConstraints();
    EXPECT_EQ(constraints.size(), 2);
}

// ========================================================================
// ConstraintSet Infeasibility Detection Tests
// ========================================================================

TEST(ConstraintSetTest, HasInfeasibleCombinationZeroAssets) {
    ConstraintSet set;
    set.add(std::make_shared<FullyInvestedConstraint>());
    EXPECT_THROW(set.hasInfeasibleCombination(0), std::invalid_argument);
}

TEST(ConstraintSetTest, HasInfeasibleCombinationNoConstraints) {
    ConstraintSet set;
    EXPECT_FALSE(set.hasInfeasibleCombination(3));
}

TEST(ConstraintSetTest, HasInfeasibleCombinationFullyInvestedOnly) {
    ConstraintSet set;
    set.add(std::make_shared<FullyInvestedConstraint>());
    EXPECT_FALSE(set.hasInfeasibleCombination(3));
}

TEST(ConstraintSetTest, HasInfeasibleCombinationFullyInvestedAndLongOnly) {
    ConstraintSet set;
    set.add(std::make_shared<FullyInvestedConstraint>());
    set.add(std::make_shared<LongOnlyConstraint>());
    EXPECT_FALSE(set.hasInfeasibleCombination(3));  // This is feasible
}

TEST(ConstraintSetTest, InfeasibleBoxUpperBoundsTooLow) {
    ConstraintSet set;
    set.add(std::make_shared<FullyInvestedConstraint>());
    set.add(std::make_shared<BoxConstraint>(0.0, 0.2));  // Max 0.2 per asset, 3 assets = max 0.6
    EXPECT_TRUE(set.hasInfeasibleCombination(3));
}

TEST(ConstraintSetTest, InfeasibleBoxLowerBoundsTooHigh) {
    ConstraintSet set;
    set.add(std::make_shared<FullyInvestedConstraint>());
    set.add(std::make_shared<BoxConstraint>(0.4, 0.6));  // Min 0.4 per asset, 3 assets = min 1.2
    EXPECT_TRUE(set.hasInfeasibleCombination(3));
}

TEST(ConstraintSetTest, FeasibleBoxConstraints) {
    ConstraintSet set;
    set.add(std::make_shared<FullyInvestedConstraint>());
    set.add(std::make_shared<BoxConstraint>(0.1, 0.5));  // 3 assets: min 0.3, max 1.5
    EXPECT_FALSE(set.hasInfeasibleCombination(3));
}

TEST(ConstraintSetTest, InfeasiblePerAssetBoundsUpperTooLow) {
    ConstraintSet set;
    set.add(std::make_shared<FullyInvestedConstraint>());
    std::vector<double> lower = {0.0, 0.0, 0.0};
    std::vector<double> upper = {0.2, 0.3, 0.3};  // Sum = 0.8 < 1.0
    set.add(std::make_shared<BoxConstraint>(lower, upper));
    EXPECT_TRUE(set.hasInfeasibleCombination(3));
}

TEST(ConstraintSetTest, InfeasiblePerAssetBoundsLowerTooHigh) {
    ConstraintSet set;
    set.add(std::make_shared<FullyInvestedConstraint>());
    std::vector<double> lower = {0.4, 0.4, 0.4};  // Sum = 1.2 > 1.0
    std::vector<double> upper = {0.5, 0.5, 0.5};
    set.add(std::make_shared<BoxConstraint>(lower, upper));
    EXPECT_TRUE(set.hasInfeasibleCombination(3));
}

TEST(ConstraintSetTest, FeasiblePerAssetBounds) {
    ConstraintSet set;
    set.add(std::make_shared<FullyInvestedConstraint>());
    std::vector<double> lower = {0.1, 0.2, 0.1};  // Sum = 0.4
    std::vector<double> upper = {0.4, 0.5, 0.5};  // Sum = 1.4
    set.add(std::make_shared<BoxConstraint>(lower, upper));
    EXPECT_FALSE(set.hasInfeasibleCombination(3));
}

TEST(ConstraintSetTest, InfeasiblePerAssetBoundsSizeMismatch) {
    ConstraintSet set;
    set.add(std::make_shared<FullyInvestedConstraint>());
    std::vector<double> lower = {0.1, 0.2};  // Only 2 bounds
    std::vector<double> upper = {0.4, 0.5};
    set.add(std::make_shared<BoxConstraint>(lower, upper));
    EXPECT_TRUE(set.hasInfeasibleCombination(3));  // 3 assets but 2 bounds
}

TEST(ConstraintSetTest, InfeasibleLongOnlyAndNegativeBoxBounds) {
    ConstraintSet set;
    set.add(std::make_shared<LongOnlyConstraint>());
    set.add(std::make_shared<BoxConstraint>(-0.5, -0.1));  // All negative
    EXPECT_TRUE(set.hasInfeasibleCombination(3));
}

TEST(ConstraintSetTest, FeasibleLongOnlyAndPositiveBoxBounds) {
    ConstraintSet set;
    set.add(std::make_shared<LongOnlyConstraint>());
    set.add(std::make_shared<BoxConstraint>(0.0, 0.5));
    EXPECT_FALSE(set.hasInfeasibleCombination(3));
}

// ========================================================================
// Integration Tests
// ========================================================================

TEST(ConstraintIntegrationTest, TypicalLongOnlyPortfolio) {
    ConstraintSet set;
    set.add(std::make_shared<FullyInvestedConstraint>());
    set.add(std::make_shared<LongOnlyConstraint>());
    set.add(std::make_shared<BoxConstraint>(0.0, 0.4));

    // Feasible portfolio
    Vector weights1({0.3, 0.35, 0.35});
    EXPECT_TRUE(set.isFeasible(weights1));

    // Violates box constraint
    Vector weights2({0.5, 0.3, 0.2});
    EXPECT_FALSE(set.isFeasible(weights2));

    // Violates fully invested
    Vector weights3({0.3, 0.3, 0.3});
    EXPECT_FALSE(set.isFeasible(weights3));

    // Violates long-only
    Vector weights4({0.6, -0.1, 0.5});
    EXPECT_FALSE(set.isFeasible(weights4));
}

TEST(ConstraintIntegrationTest, LongShortPortfolio) {
    ConstraintSet set;
    set.add(std::make_shared<FullyInvestedConstraint>());
    set.add(std::make_shared<BoxConstraint>(-0.5, 0.5));

    // Feasible with shorts within bounds, sums to 1.0
    Vector weights1({0.5, -0.3, 0.8});
    EXPECT_FALSE(set.isFeasible(weights1));  // 0.8 exceeds upper bound

    // Actually feasible portfolio
    Vector weights2({0.5, 0.0, 0.5});
    EXPECT_TRUE(set.isFeasible(weights2));

    // Violates box constraint (short too large)
    Vector weights3({0.8, -0.6, 0.8});
    EXPECT_FALSE(set.isFeasible(weights3));
}

TEST(ConstraintIntegrationTest, ConcentratedPortfolio) {
    ConstraintSet set;
    set.add(std::make_shared<FullyInvestedConstraint>());
    set.add(std::make_shared<LongOnlyConstraint>());

    // Highly concentrated but feasible
    Vector weights({0.9, 0.05, 0.05});
    EXPECT_TRUE(set.isFeasible(weights));
}

TEST(ConstraintIntegrationTest, EquallyWeightedPortfolio) {
    ConstraintSet set;
    set.add(std::make_shared<FullyInvestedConstraint>());
    set.add(std::make_shared<LongOnlyConstraint>());
    set.add(std::make_shared<BoxConstraint>(0.0, 0.5));

    size_t numAssets = 5;
    Vector weights(numAssets, 1.0 / numAssets);
    EXPECT_TRUE(set.isFeasible(weights));
}

TEST(ConstraintIntegrationTest, RealWorldScenario) {
    // Real-world scenario: 10-asset portfolio with various constraints
    ConstraintSet set;
    set.add(std::make_shared<FullyInvestedConstraint>(1e-6));
    set.add(std::make_shared<LongOnlyConstraint>());

    // Different bounds for different asset classes
    std::vector<double> lower = {0.05, 0.05, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00};
    std::vector<double> upper = {0.30, 0.30, 0.25, 0.25, 0.25, 0.15, 0.15, 0.15, 0.10, 0.10};
    set.add(std::make_shared<BoxConstraint>(lower, upper));

    // Check that constraint set is feasible
    EXPECT_FALSE(set.hasInfeasibleCombination(10));

    // Test a feasible allocation
    Vector weights({0.20, 0.20, 0.15, 0.10, 0.10, 0.10, 0.05, 0.05, 0.03, 0.02});
    EXPECT_TRUE(set.isFeasible(weights));
}
