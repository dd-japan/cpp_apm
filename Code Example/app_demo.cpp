#include <iostream>
#include <stdexcept>
#include <string>

// Simulates a nested operation that throws an exception
void verify_token() {
  std::cout << "[verify_token] Verifying JWT token..." << std::endl;
}

void authenticate_user() {
  std::cout << "[authenticate_user] Starting authentication..." << std::endl;
  verify_token();
  std::cout << "[authenticate_user] Authentication successful." << std::endl;
}

void fetch_from_db(const std::string& user_id) {
  std::cout << "[fetch_from_db] Fetching user profile for user_id = " << user_id << std::endl;
}

void load_user_profile(const std::string& user_id) {
  std::cout << "[load_user_profile] Loading user profile for user_id = " << user_id << std::endl;
  fetch_from_db(user_id);
  std::cout << "[load_user_profile] Profile loaded for user_id = " << user_id << std::endl;
}

void validate_input() {
  std::cout << "[validate_input] Validating form input..." << std::endl;
}

void risky_operation() {
  std::cout << "[risky_operation] Calling external billing API..." << std::endl;
  throw std::runtime_error("Billing API timeout");
}

void process_data() {
  std::cout << "[process_data] Processing payment data..." << std::endl;
  validate_input();

  try {
    risky_operation();
  } catch (const std::exception& ex) {
    std::cerr << "[process_data] ERROR: " << ex.what() << std::endl;
  }
}

int main() {
  std::cout << "[main] Starting user session workflow..." << std::endl;

  std::string user_id = "user123";  // Example user ID

  authenticate_user();
  load_user_profile(user_id);
  process_data();

  std::cout << "[main] Session workflow completed." << std::endl;
  return 0;
}
