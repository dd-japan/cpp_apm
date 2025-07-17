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

void fetch_from_db() {
  std::cout << "[fetch_from_db] Fetching user profile from database..." << std::endl;
}

void load_user_profile() {
  std::cout << "[load_user_profile] Loading user profile..." << std::endl;
  fetch_from_db();
  std::cout << "[load_user_profile] Profile loaded." << std::endl;
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

  authenticate_user();
  load_user_profile();
  process_data();

  std::cout << "[main] Session workflow completed." << std::endl;
  return 0;
}
