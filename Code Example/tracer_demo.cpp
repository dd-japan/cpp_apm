#include <datadog/tracer.h>      // Main tracing API
#include <datadog/config.h>      // Configuration support
#include <iostream>
#include <stdexcept>
#include <string>

// Set shorter namespace alias
namespace dd = datadog::tracing;

// Create a child span under the provided parent span.
// This models the internal step "verify_token" as part of a larger operation.
void verify_token(dd::Span& parent_span) {
  auto span = parent_span.create_child();            //start a child span
  span.set_name("verify_token");                         // set a span attribute(Logical operation name)
  span.set_resource_name("auth.verify_token");           // set a span attribute(Trace resource name for grouping)
  span.set_tag("auth.type", "JWT");                      // set a span tag(Custom tag: token type)

  std::cout << "[verify_token] Verifying JWT token..." << std::endl;
}

// Models user authentication as a span, and links nested verification span.
void authenticate_user(dd::Span& parent_span) {
  auto span = parent_span.create_child();    //start a child span
  span.set_name("authenticate_user");         //set a span attribute 
  span.set_resource_name("auth.login");     //set a span attribute
  span.set_tag("auth.method", "password");    //set a span tag

  std::cout << "[authenticate_user] Starting authentication..." << std::endl;
  verify_token(span);                                    // Pass this span as parent to nested function
  std::cout << "[authenticate_user] Authentication successful." << std::endl;
}

// Models a database fetch with a traceable span.
void fetch_from_db(dd::Span& parent_span, const std::string& user_id) {
  auto span = parent_span.create_child(); //start a child span
  span.set_name("fetch_from_db");         //set a span attribute 
  span.set_resource_name("db.query");     //set a span attribute
  span.set_tag("db.table", "users");      //set a span tag
  span.set_tag("user_id", user_id);       //add user_id to span for context
  
  std::cout << "[fetch_from_db] Fetching user profile for user_id = " << user_id << std::endl;
}

// High-level span for loading user profile; internally calls DB layer.
void load_user_profile(dd::Span& parent_span, const std::string& user_id) {
  auto span = parent_span.create_child();  //start a child span
  span.set_name("load_user_profile");          //set a span attribute 
  span.set_resource_name("user.load_profile");  //set a span attribute
  span.set_tag("profile.source", "primary");     //set a span tag
  span.set_tag("user_id", user_id);             //set user_id tag

  std::cout << "[load_user_profile] Loading user profile for user_id = " << user_id << std::endl;
  fetch_from_db(span, user_id);
  std::cout << "[load_user_profile] Profile loaded for user_id = " << user_id << std::endl;
}

// Validates user input; modeled as a standalone span.
void validate_input(dd::Span& parent_span) {
  auto span = parent_span.create_child();   //start a child span
  span.set_name("validate_input");           //set a span attribute 
  span.set_resource_name("form.validate");   //set a span attribute
  span.set_tag("form.type", "payment");     //set a span tag

  std::cout << "[validate_input] Validating form input..." << std::endl;
}

// Simulates an external service call that throws an error.
// The span will be created before the exception.
void risky_operation(dd::Span& parent_span) {
  auto span = parent_span.create_child();     //start a child span
  span.set_name("risky_operation");           //set a span attribute
  span.set_resource_name("external.billing_call");  //set a span attribute
  span.set_tag("service", "billing-api");  //set a span tag

  std::cout << "[risky_operation] Calling external billing API..." << std::endl;

  throw std::runtime_error("Billing API timeout");       // This will be caught in the caller
}

// Main business logic handler that captures and marks exceptions as span errors.
void process_data(dd::Span& parent_span) {
  auto span = parent_span.create_child();     //start a child span
  span.set_name("process_data");              //set a span attribute
  span.set_resource_name("business.process"); //set a span attribute
  span.set_tag("component", "payment");       //set a span tag

  std::cout << "[process_data] Processing payment data..." << std::endl;
  validate_input(span);

  try {
    risky_operation(span);
  } catch (const std::exception& ex) {
    std::cerr << "[process_data] ERROR: " << ex.what() << std::endl;

    // Annotate the current span with error details for Datadog APM UI
    span.set_error_message(ex.what());
    span.set_error_type("std::runtime_error");
    span.set_error_stack("risky_operation() stack trace");
    span.set_tag("error.origin", "risky_operation");
  }
}

int main() {
  dd::TracerConfig config;                               // To define the config of the root trace
  config.service = "cpp-payment-app";                    // The service name shown in Datadog

  // Validate and apply tracer config
  const auto validated = dd::finalize_config(config);
  if (!validated) {
    std::cerr << "[config error] " << validated.error() << std::endl;
    return 1;
  }

  dd::Tracer tracer{*validated};                        // Define a tracer from the main entry

  // Create root span for the entire request/session context
  auto root_span = tracer.create_span();            // Create the root span
  root_span.set_name("main_flow");                  // Set a root span attribute(Logical operation name)
  root_span.set_resource_name("user_session");      // Set a root span attribute(Trace resource name for grouping)

  std::cout << "[main] Starting user session workflow..." << std::endl;

  std::string user_id = "user123";                  // User ID as a parameter

  // Pass root span into child business logic functions
  authenticate_user(root_span);
  load_user_profile(root_span, user_id);
  process_data(root_span);

  std::cout << "[main] Session workflow completed." << std::endl;
  return 0;
}
