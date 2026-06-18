#include "ast/printers.h"
#include "ast/query_optimizer.h"
#include "dsl/query.h"

#ifdef RELALG_GUI_DISABLED

#include <iostream>

int main() {
    ra::ExprPtr query = ra::Table("users", {"id", "name", "age"}) |
                        ra::Sigma("age > 20") | ra::Pi({"name", "age"});
    ra::RelationalAlgebraPrinter printer;
    std::cout << printer.print(query) << '\n';
    return 0;
}

#else

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <GLFW/glfw3.h>

#include <exception>
#include <cstdio>
#include <string>

namespace {

struct DemoExample {
    const char *name;
    const char *dsl;
    ra::ExprPtr expr;
};

ra::ExprPtr buildUsersProjection() {
    return ra::Table("users", {"id", "name", "age", "city"}) |
           ra::Sigma(ra::Gt("age", ra::Literal(20))) | ra::Pi({"name", "age"});
}

ra::ExprPtr buildOrdersJoin() {
    ra::ExprPtr users = ra::Table("users", {"id", "name", "city"});
    ra::ExprPtr orders = ra::Table("orders", {"order_id", "user_id", "total"});
    return users | ra::Join(orders, "id", "user_id") | ra::Pi({"name", "order_id", "total"});
}

ra::ExprPtr buildRenamedSelection() {
    return ra::Table("students", {"id", "group", "score"}) | ra::Rho("s") |
           ra::Sigma("score >= 80") | ra::Pi({"id", "group", "score"});
}

DemoExample makeExample(int index) {
    if (index == 1) {
        return {"users join orders",
                "Table(\"users\", {\"id\", \"name\", \"city\"}) | "
                "Join(Table(\"orders\", {\"order_id\", \"user_id\", \"total\"}), "
                "\"id\", \"user_id\") | Pi({\"name\", \"order_id\", \"total\"})",
                buildOrdersJoin()};
    }
    if (index == 2) {
        return {"renamed students",
                "Table(\"students\", {\"id\", \"group\", \"score\"}) | Rho(\"s\") | "
                "Sigma(\"score >= 80\") | Pi({\"id\", \"group\", \"score\"})",
                buildRenamedSelection()};
    }
    return {"users projection",
            "Table(\"users\", {\"id\", \"name\", \"age\", \"city\"}) | "
            "Sigma(Gt(\"age\", Literal(20))) | Pi({\"name\", \"age\"})",
            buildUsersProjection()};
}

void renderSequence(const ra::StringSequence &sequence) {
    for (int index = 0; index < sequence.get_count(); index++) {
        ImGui::BulletText("%s", sequence.get(index).c_str());
    }
}

void renderDslAstTab(int &selected_example) {
    const char *names[] = {"users projection", "users join orders", "renamed students"};

    ImGui::BeginChild("examples", ImVec2(220.0F, 0.0F), true);
    ImGui::TextUnformatted("Примеры");
    ImGui::Separator();
    for (int index = 0; index < 3; index++) {
        if (ImGui::Selectable(names[index], selected_example == index)) {
            selected_example = index;
        }
    }
    ImGui::EndChild();

    ImGui::SameLine();

    ImGui::BeginChild("details", ImVec2(0.0F, 0.0F), true);
    DemoExample example = makeExample(selected_example);
    ra::RelationalAlgebraPrinter algebra_printer;
    ra::AstTreePrinter tree_printer;

    ImGui::Text("Запрос: %s", example.name);
    ImGui::Separator();

    if (ImGui::BeginTabBar("views")) {
        if (ImGui::BeginTabItem("DSL")) {
            ImGui::TextWrapped("%s", example.dsl);
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("RA")) {
            const std::string algebra = algebra_printer.print(example.expr);
            ImGui::TextWrapped("%s", algebra.c_str());
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("AST")) {
            const std::string tree = tree_printer.print(example.expr);
            ImGui::TextUnformatted(tree.c_str());
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Schema")) {
            renderSequence(example.expr->schema());
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }

    ImGui::EndChild();
}

void renderOptimizerTab() {
    DemoExample example = makeExample(1);
    ra::QueryOptimizer optimizer;
    optimizer.optimize(example.expr);

    ImGui::TextUnformatted("Итерация 2: QueryOptimizer");
    ImGui::Separator();
    ImGui::TextWrapped("На первом шаге оптимизатор подключен только как visitor-заглушка.");
    renderSequence(optimizer.notes());
}

void renderBackendsTab() {
    ImGui::TextUnformatted("Итерация 3: CSV и SQL backends");
    ImGui::Separator();
    ImGui::TextWrapped("Исполнение запросов, CSV и SQLite будут подключены после стабилизации AST и оптимизатора.");
}

void glfwErrorCallback(int error, const char *description) {
    std::fprintf(stderr, "GLFW error %d: %s\n", error, description);
}

}  // namespace

int main() {
    glfwSetErrorCallback(glfwErrorCallback);
    if (!glfwInit()) {
        return 1;
    }

#if defined(__APPLE__)
    const char *glsl_version = "#version 150";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#else
    const char *glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
#endif

    GLFWwindow *window = glfwCreateWindow(1180, 760, "Relational Algebra DSL", nullptr, nullptr);
    if (window == nullptr) {
        glfwTerminate();
        return 1;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImGui::StyleColorsLight();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    int selected_example = 0;

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::SetNextWindowPos(ImVec2(0.0F, 0.0F), ImGuiCond_Always);
        ImGui::SetNextWindowSize(io.DisplaySize, ImGuiCond_Always);
        ImGui::Begin("Relational Algebra Lab", nullptr,
                     ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove |
                         ImGuiWindowFlags_NoResize);

        ImGui::TextUnformatted("DSL и AST реляционной алгебры");
        ImGui::Separator();
        if (ImGui::BeginTabBar("main-tabs")) {
            if (ImGui::BeginTabItem("DSL/AST")) {
                renderDslAstTab(selected_example);
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Optimizer")) {
                renderOptimizerTab();
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Backends")) {
                renderBackendsTab();
                ImGui::EndTabItem();
            }
            ImGui::EndTabBar();
        }
        ImGui::End();

        ImGui::Render();
        int display_w = 0;
        int display_h = 0;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.94F, 0.95F, 0.96F, 1.0F);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

#endif
