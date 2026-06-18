#include "ast/printers.h"
#include "backend/csv/csv_backend.h"
#include "backend/csv/csv_parser.h"
#include "backend/sql/sqlite_backend.h"
#include "dsl/query.h"
#include "query_optimizer/query_optimizer.h"

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

#include <cstdio>
#include <exception>
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
  return users | ra::Join(orders, "id", "user_id") |
         ra::Pi({"name", "order_id", "total"});
}

ra::ExprPtr buildRenamedSelection() {
  return ra::Table("students", {"id", "group", "score"}) | ra::Rho("s") |
         ra::Sigma("score >= 80") | ra::Pi({"id", "group", "score"});
}

ra::ExprPtr buildPushDownSelection() {
  ra::ExprPtr users =
      ra::Table("users", {"users.id", "users.name", "users.age"});
  ra::ExprPtr orders =
      ra::Table("orders", {"orders.id", "orders.user_id", "orders.total"});
  return users | ra::Join(orders, "users.id", "orders.user_id") |
         ra::Sigma(ra::Gt("users.age", ra::Literal(20))) |
         ra::Pi({"users.name", "orders.total"});
}

ra::ExprPtr buildProjectionPruning() {
  ra::ExprPtr users =
      ra::Table("users", {"users.id", "users.name", "users.age"});
  ra::ExprPtr orders =
      ra::Table("orders", {"orders.id", "orders.user_id", "orders.total"});
  return users | ra::Join(orders, "users.id", "orders.user_id") |
         ra::Pi({"users.name"});
}

int exampleCount() { return 5; }

const char *exampleName(int index) {
  if (index == 1) {
    return "users join orders";
  }
  if (index == 2) {
    return "renamed students";
  }
  if (index == 3) {
    return "push-down selection";
  }
  if (index == 4) {
    return "projection pruning";
  }
  return "users projection";
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
    return {
        "renamed students",
        "Table(\"students\", {\"id\", \"group\", \"score\"}) | Rho(\"s\") | "
        "Sigma(\"score >= 80\") | Pi({\"id\", \"group\", \"score\"})",
        buildRenamedSelection()};
  }
  if (index == 3) {
    return {"push-down selection",
            "Table(\"users\", {\"users.id\", \"users.name\", \"users.age\"}) | "
            "Join(Table(\"orders\", {\"orders.id\", \"orders.user_id\", "
            "\"orders.total\"}), "
            "\"users.id\", \"orders.user_id\") | Sigma(Gt(\"users.age\", "
            "Literal(20))) | "
            "Pi({\"users.name\", \"orders.total\"})",
            buildPushDownSelection()};
  }
  if (index == 4) {
    return {"projection pruning",
            "Table(\"users\", {\"users.id\", \"users.name\", \"users.age\"}) | "
            "Join(Table(\"orders\", {\"orders.id\", \"orders.user_id\", "
            "\"orders.total\"}), "
            "\"users.id\", \"orders.user_id\") | Pi({\"users.name\"})",
            buildProjectionPruning()};
  }
  return {"users projection",
          "Table(\"users\", {\"id\", \"name\", \"age\", \"city\"}) | "
          "Sigma(Gt(\"age\", Literal(20))) | Pi({\"name\", \"age\"})",
          buildUsersProjection()};
}

void renderSequence(const Sequence<std::string> &sequence) {
  for (int index = 0; index < sequence.get_count(); index++) {
    ImGui::BulletText("%s", sequence.get(index).c_str());
  }
}

void renderDslAstTab(int &selected_example) {
  ImGui::BeginChild("examples", ImVec2(220.0F, 0.0F), true);
  ImGui::TextUnformatted("Примеры");
  ImGui::Separator();
  for (int index = 0; index < exampleCount(); index++) {
    if (ImGui::Selectable(exampleName(index), selected_example == index)) {
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

void renderOptimizerTab(int selected_example) {
  DemoExample example = makeExample(selected_example);
  ra::QueryOptimizer optimizer;
  ra::OptimizationResult result = optimizer.optimize(example.expr);
  ra::RelationalAlgebraPrinter algebra_printer;
  ra::AstTreePrinter tree_printer;

  ImGui::TextUnformatted("Итерация 2: QueryOptimizer");
  ImGui::Separator();
  ImGui::Text("Пример: %s", example.name);

  if (ImGui::BeginTabBar("optimizer-views")) {
    if (ImGui::BeginTabItem("RA до/после")) {
      const std::string original = algebra_printer.print(result.original());
      const std::string optimized = algebra_printer.print(result.optimized());
      ImGui::TextUnformatted("До:");
      ImGui::TextWrapped("%s", original.c_str());
      ImGui::Separator();
      ImGui::TextUnformatted("После:");
      ImGui::TextWrapped("%s", optimized.c_str());
      ImGui::EndTabItem();
    }
    if (ImGui::BeginTabItem("AST до/после")) {
      const std::string original = tree_printer.print(result.original());
      const std::string optimized = tree_printer.print(result.optimized());
      ImGui::TextUnformatted("До:");
      ImGui::TextUnformatted(original.c_str());
      ImGui::Separator();
      ImGui::TextUnformatted("После:");
      ImGui::TextUnformatted(optimized.c_str());
      ImGui::EndTabItem();
    }
    if (ImGui::BeginTabItem("Отчет")) {
      ImGui::TextUnformatted("Примененные правила и предупреждения");
      ImGui::Separator();
      renderSequence(result.report().notes());
      ImGui::EndTabItem();
    }
    ImGui::EndTabBar();
  }
}

std::string dataPath(const std::string &name) {
  return std::string(RELALG_DATA_DIR) + "/" + name;
}

ra::SourceCatalog makeDemoCatalog() {
  ra::SourceCatalog catalog;
  const auto users = ra::makeSequence({"users.id", "users.name", "users.age"});
  const auto orders =
      ra::makeSequence({"orders.id", "orders.user_id", "orders.total"});
  catalog.addCsv("users", dataPath("users.csv"), users);
  catalog.addCsv("orders", dataPath("orders.csv"), orders);
  catalog.addSQLite("users", dataPath("demo.sqlite3"), "users", users);
  catalog.addSQLite("orders", dataPath("demo.sqlite3"), "orders", orders);
  return catalog;
}

void renderResultTable(const ra::QueryResult &result, const char *id) {
  const int column_count = result.schema().get_count();
  if (ImGui::BeginTable(id, column_count,
                        ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg |
                            ImGuiTableFlags_Resizable | ImGuiTableFlags_ScrollY,
                        ImVec2(0.0F, 190.0F))) {
    for (int column = 0; column < column_count; column++) {
      ImGui::TableSetupColumn(result.schema().get(column).c_str());
    }
    ImGui::TableHeadersRow();
    for (int row = 0; row < result.rows().get_count(); row++) {
      ImGui::TableNextRow();
      for (int column = 0; column < column_count; column++) {
        ImGui::TableSetColumnIndex(column);
        ImGui::TextUnformatted(
            result.rows().get(row).cell(column).display().c_str());
      }
    }
    ImGui::EndTable();
  }
}

void renderSourcePreview(const ra::SourceCatalog &catalog) {
  if (!ImGui::CollapsingHeader("Исходные документы и таблицы")) {
    return;
  }
  ra::CsvParser parser;
  if (ImGui::BeginTabBar("source-preview")) {
    const char *names[] = {"users", "orders"};
    for (const char *name : names) {
      if (ImGui::BeginTabItem(name)) {
        const ra::SourceBinding *source =
            catalog.find(name, ra::SourceKind::Csv);
        if (source != nullptr) {
          const ra::QueryResult data =
              parser.read(source->location(), source->schema());
          ImGui::TextUnformatted(source->location().c_str());
          renderResultTable(data, name);
        }
        ImGui::EndTabItem();
      }
    }
    ImGui::EndTabBar();
  }
}

void renderParameters(const Sequence<ra::SqlParameter> &parameters) {
  if (parameters.get_count() == 0) {
    ImGui::TextDisabled("Параметров нет");
    return;
  }
  for (int index = 0; index < parameters.get_count(); index++) {
    ImGui::BulletText("?%d = %s", index + 1,
                      parameters.get(index).value().c_str());
  }
}

void renderBackendsTab() {
  static int backend = 0;
  static bool optimize = true;
  static bool has_run = true;
  static bool select_execution = true;

  const char *dsl = "Table(\"users\", ...) | Join(Table(\"orders\", ...), "
                    "\"users.id\", \"orders.user_id\") | "
                    "Sigma(Gt(\"users.age\", Literal(20))) | "
                    "Pi({\"users.name\", \"orders.total\"})";
  const ra::ExprPtr original = buildPushDownSelection();
  ra::QueryOptimizer optimizer;
  const ra::OptimizationResult optimization = optimizer.optimize(original);
  const ra::ExprPtr executable = optimize ? optimization.optimized() : original;
  const ra::SourceCatalog catalog = makeDemoCatalog();
  ra::RelationalAlgebraPrinter algebra_printer;
  ra::AstTreePrinter tree_printer;

  ImGui::TextUnformatted("DSL -> AST -> QueryOptimizer -> Backend -> Result");
  ImGui::Separator();
  ImGui::RadioButton("CSV", &backend, 0);
  ImGui::SameLine();
  ImGui::RadioButton("SQLite", &backend, 1);
  ImGui::SameLine();
  ImGui::Checkbox("Использовать оптимизатор", &optimize);
  ImGui::SameLine();
  if (ImGui::Button("Запустить")) {
    has_run = true;
  }

  if (ImGui::BeginTabBar("backend-flow")) {
    if (ImGui::BeginTabItem("DSL и AST")) {
      ImGui::TextWrapped("%s", dsl);
      ImGui::Separator();
      const std::string ast = tree_printer.print(original);
      ImGui::TextUnformatted(ast.c_str());
      ImGui::EndTabItem();
    }
    if (ImGui::BeginTabItem("Оптимизация")) {
      const std::string before = algebra_printer.print(original);
      const std::string after = algebra_printer.print(optimization.optimized());
      ImGui::TextUnformatted("До");
      ImGui::TextWrapped("%s", before.c_str());
      ImGui::Separator();
      ImGui::TextUnformatted("После");
      ImGui::TextWrapped("%s", after.c_str());
      renderSequence(optimization.report().notes());
      ImGui::EndTabItem();
    }
    const ImGuiTabItemFlags execution_flags =
        select_execution ? ImGuiTabItemFlags_SetSelected
                         : ImGuiTabItemFlags_None;
    if (ImGui::BeginTabItem("Исполнение", nullptr, execution_flags)) {
      if (!has_run) {
        ImGui::TextDisabled("Нажмите Запустить для выполнения запроса");
      } else {
        try {
          if (backend == 0) {
            const ra::CsvExecutionResult execution =
                ra::CsvBackend(catalog).execute(executable);
            ImGui::TextUnformatted("План CSV");
            renderSequence(execution.plan());
            ImGui::Separator();
            ImGui::Text("Результат: %d строк", execution.result().rowCount());
            renderResultTable(execution.result(), "csv-result");
          } else {
            const ra::SqlExecutionResult execution =
                ra::SQLiteBackend(catalog).execute(executable);
            ImGui::TextUnformatted("Сгенерированный SQL");
            ImGui::TextWrapped("%s", execution.sql().text().c_str());
            ImGui::TextUnformatted("Bind-параметры");
            renderParameters(execution.sql().parameters());
            ImGui::Separator();
            ImGui::Text("Результат: %d строк", execution.result().rowCount());
            renderResultTable(execution.result(), "sql-result");
          }
        } catch (const std::exception &error) {
          ImGui::TextColored(ImVec4(0.75F, 0.15F, 0.12F, 1.0F), "%s",
                             error.what());
        }
      }
      ImGui::EndTabItem();
    }
    ImGui::EndTabBar();
    select_execution = false;
  }
  renderSourcePreview(catalog);
}

void glfwErrorCallback(int error, const char *description) {
  std::fprintf(stderr, "GLFW error %d: %s\n", error, description);
}

void loadCyrillicFont(ImGuiIO &io) {
#if defined(__APPLE__)
  const char *font_path = "/System/Library/Fonts/Supplemental/Arial.ttf";
#elif defined(_WIN32)
  const char *font_path = "C:\\Windows\\Fonts\\arial.ttf";
#else
  const char *font_path = "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf";
#endif
  if (io.Fonts->AddFontFromFileTTF(font_path, 16.0F, nullptr,
                                   io.Fonts->GetGlyphRangesCyrillic()) ==
      nullptr) {
    io.Fonts->AddFontDefault();
  }
}

} // namespace

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

  GLFWwindow *window =
      glfwCreateWindow(1180, 760, "Relational Algebra DSL", nullptr, nullptr);
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
  loadCyrillicFont(io);

  ImGui::StyleColorsLight();
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init(glsl_version);

  int selected_example = 0;
  bool select_backends = true;

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
        renderOptimizerTab(selected_example);
        ImGui::EndTabItem();
      }
      const ImGuiTabItemFlags backend_flags =
          select_backends ? ImGuiTabItemFlags_SetSelected
                          : ImGuiTabItemFlags_None;
      if (ImGui::BeginTabItem("Backends", nullptr, backend_flags)) {
        renderBackendsTab();
        ImGui::EndTabItem();
      }
      ImGui::EndTabBar();
      select_backends = false;
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
