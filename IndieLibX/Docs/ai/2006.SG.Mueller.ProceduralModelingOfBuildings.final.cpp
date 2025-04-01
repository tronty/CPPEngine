#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <memory>

// Define a basic Shape class
class Shape {
public:
    std::string symbol;
    std::vector<double> position; // Position in 3D space
    std::vector<double> size;     // Size of the shape
    std::vector<double> rotation; // Rotation angles

    Shape(const std::string& sym, const std::vector<double>& pos, const std::vector<double>& sz, const std::vector<double>& rot)
        : symbol(sym), position(pos), size(sz), rotation(rot) {}

    virtual void applyRule() = 0; // Pure virtual function for applying rules
    virtual ~Shape() = default;
};

// Define a basic production rule
class ProductionRule {
public:
    std::string predecessor;
    std::string successor;
    double probability;

    ProductionRule(const std::string& pred, const std::string& succ, double prob)
        : predecessor(pred), successor(succ), probability(prob) {}
};

// Define a grammar system
class CGA_Shape {
private:
    std::vector<std::shared_ptr<Shape>> shapes;
    std::map<std::string, std::vector<ProductionRule>> rules;

public:
    void addShape(const std::shared_ptr<Shape>& shape) {
        shapes.push_back(shape);
    }

    void addRule(const std::string& pred, const std::string& succ, double prob) {
        rules[pred].emplace_back(pred, succ, prob);
    }

    void applyRules() {
        for (auto& shape : shapes) {
            if (shape->symbol == "A") { // Example rule application
                shape->applyRule();
            }
        }
    }

    void printShapes() const {
        for (const auto& shape : shapes) {
            std::cout << "Shape: " << shape->symbol << ", Position: (" 
                      << shape->position[0] << ", " << shape->position[1] << ", " << shape->position[2] << ")\n";
        }
    }
};

// Example of a specific shape class
class Cube : public Shape {
public:
    Cube(const std::vector<double>& pos, const std::vector<double>& sz, const std::vector<double>& rot)
        : Shape("Cube", pos, sz, rot) {}

    void applyRule() override {
        std::cout << "Applying rule to Cube\n";
        // Example rule: Split the cube into smaller cubes
        size[0] /= 2;
        size[1] /= 2;
        size[2] /= 2;
    }
};

int main() {
    CGA_Shape grammar;

    // Create an initial shape (axiom)
    auto initialCube = std::make_shared<Cube>(std::vector<double>{0, 0, 0}, std::vector<double>{10, 10, 10}, std::vector<double>{0, 0, 0});
    grammar.addShape(initialCube);

    // Add a production rule
    grammar.addRule("A", "B", 1.0); // Example rule: Replace A with B

    // Apply the rules
    grammar.applyRules();

    // Print the shapes
    grammar.printShapes();

    return 0;
}
