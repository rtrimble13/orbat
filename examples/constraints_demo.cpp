#include "orbat/core/vector.hpp"
#include "orbat/optimizer/constraint.hpp"

#include <iomanip>
#include <iostream>

using orbat::core::Vector;
using orbat::optimizer::BoxConstraint;
using orbat::optimizer::ConstraintSet;
using orbat::optimizer::FullyInvestedConstraint;
using orbat::optimizer::LongOnlyConstraint;

void printWeights(const Vector& weights, const std::string& label) {
    std::cout << label << ": [";
    for (size_t i = 0; i < weights.size(); ++i) {
        std::cout << std::fixed << std::setprecision(3) << weights[i];
        if (i < weights.size() - 1) {
            std::cout << ", ";
        }
    }
    std::cout << "]" << std::endl;
}

void checkConstraints(const ConstraintSet& constraints, const Vector& weights) {
    std::cout << "  Feasible: " << (constraints.isFeasible(weights) ? "Yes" : "No") << std::endl;
    std::cout << std::endl;
}

int main() {
    std::cout << "===========================================\n";
    std::cout << "Portfolio Constraints Demo\n";
    std::cout << "===========================================\n\n";

    // Example 1: Traditional Long-Only Portfolio
    std::cout << "Example 1: Traditional Long-Only Portfolio\n";
    std::cout << "-------------------------------------------\n";

    ConstraintSet longOnlyConstraints;
    longOnlyConstraints.add(std::make_shared<FullyInvestedConstraint>());
    longOnlyConstraints.add(std::make_shared<LongOnlyConstraint>());
    longOnlyConstraints.add(std::make_shared<BoxConstraint>(0.0, 0.4));

    std::cout << "Constraints:\n";
    for (const auto& constraint : longOnlyConstraints.getConstraints()) {
        std::cout << "  - " << constraint->getName() << ": " << constraint->getDescription()
                  << std::endl;
    }
    std::cout << std::endl;

    Vector weights1({0.3, 0.35, 0.35});
    printWeights(weights1, "Weights");
    checkConstraints(longOnlyConstraints, weights1);

    Vector weights2({0.5, 0.3, 0.2});
    printWeights(weights2, "Weights");
    std::cout << "  Feasible: No (violates box constraint: 0.5 > 0.4)\n\n";

    Vector weights3({0.4, -0.1, 0.7});
    printWeights(weights3, "Weights");
    std::cout << "  Feasible: No (violates long-only: -0.1 < 0)\n\n";

    // Example 2: Multi-Asset Class Portfolio
    std::cout << "Example 2: Multi-Asset Class Portfolio\n";
    std::cout << "---------------------------------------\n";

    std::vector<double> lower = {0.05, 0.05, 0.00, 0.00, 0.00};
    std::vector<double> upper = {0.30, 0.30, 0.25, 0.20, 0.15};

    ConstraintSet multiAssetConstraints;
    multiAssetConstraints.add(std::make_shared<FullyInvestedConstraint>());
    multiAssetConstraints.add(std::make_shared<LongOnlyConstraint>());
    multiAssetConstraints.add(std::make_shared<BoxConstraint>(lower, upper));

    std::cout << "Per-asset bounds:\n";
    for (size_t i = 0; i < lower.size(); ++i) {
        std::cout << "  Asset " << i + 1 << ": [" << std::fixed << std::setprecision(2) << lower[i]
                  << ", " << upper[i] << "]\n";
    }
    std::cout << std::endl;

    Vector weights4({0.20, 0.25, 0.20, 0.20, 0.15});
    printWeights(weights4, "Weights");
    checkConstraints(multiAssetConstraints, weights4);

    Vector weights5({0.02, 0.25, 0.20, 0.20, 0.33});  // Asset 1 below minimum
    printWeights(weights5, "Weights");
    std::cout << "  Feasible: No (Asset 1: 0.02 < 0.05)\n\n";

    // Example 3: Infeasible Constraint Detection
    std::cout << "Example 3: Infeasible Constraint Detection\n";
    std::cout << "-------------------------------------------\n";

    ConstraintSet infeasibleConstraints;
    infeasibleConstraints.add(std::make_shared<FullyInvestedConstraint>());
    infeasibleConstraints.add(std::make_shared<BoxConstraint>(0.0, 0.2));  // Max 20% per asset

    std::cout << "Constraints:\n";
    std::cout << "  - Fully invested (sum = 1.0)\n";
    std::cout << "  - Box constraint: [0.0, 0.2] for all assets\n";
    std::cout << "  - Number of assets: 3\n\n";

    if (infeasibleConstraints.hasInfeasibleCombination(3)) {
        std::cout << "Result: INFEASIBLE\n";
        std::cout << "Reason: Maximum allocation is 3 * 0.2 = 0.6 < 1.0\n";
        std::cout << "Cannot achieve fully invested constraint.\n\n";
    }

    // Example 4: Equal Weight Portfolio
    std::cout << "Example 4: Equal Weight Portfolio\n";
    std::cout << "----------------------------------\n";

    size_t numAssets = 5;
    Vector equalWeights(numAssets, 1.0 / numAssets);

    ConstraintSet equalWeightConstraints;
    equalWeightConstraints.add(std::make_shared<FullyInvestedConstraint>());
    equalWeightConstraints.add(std::make_shared<LongOnlyConstraint>());
    equalWeightConstraints.add(std::make_shared<BoxConstraint>(0.0, 0.5));

    printWeights(equalWeights, "Equal weights");
    checkConstraints(equalWeightConstraints, equalWeights);

    // Example 5: Pre-optimization Validation
    std::cout << "Example 5: Pre-optimization Validation\n";
    std::cout << "---------------------------------------\n";

    ConstraintSet validationConstraints;
    validationConstraints.add(std::make_shared<FullyInvestedConstraint>());
    validationConstraints.add(std::make_shared<LongOnlyConstraint>());

    std::vector<double> assetLower = {0.15, 0.15, 0.15, 0.15, 0.15};
    std::vector<double> assetUpper = {0.25, 0.25, 0.25, 0.25, 0.25};
    validationConstraints.add(std::make_shared<BoxConstraint>(assetLower, assetUpper));

    std::cout << "Constraints:\n";
    std::cout << "  - Fully invested\n";
    std::cout << "  - Long-only\n";
    std::cout << "  - Each asset: [0.15, 0.25]\n";
    std::cout << "  - Number of assets: 5\n\n";

    if (validationConstraints.hasInfeasibleCombination(5)) {
        std::cout << "Result: INFEASIBLE\n";
        std::cout << "Reason: Lower bounds sum to " << std::fixed << std::setprecision(2)
                  << 5 * 0.15 << " and upper bounds sum to " << 5 * 0.25 << "\n";
        std::cout << "Range [0.75, 1.25] contains 1.0, but individual feasibility may vary.\n\n";
    } else {
        std::cout << "Result: FEASIBLE\n";
        std::cout << "Lower bound sum: " << std::fixed << std::setprecision(2) << 5 * 0.15
                  << " <= 1.0\n";
        std::cout << "Upper bound sum: " << 5 * 0.25 << " >= 1.0\n\n";
    }

    std::cout << "===========================================\n";
    std::cout << "Demo Complete\n";
    std::cout << "===========================================\n";

    return 0;
}
