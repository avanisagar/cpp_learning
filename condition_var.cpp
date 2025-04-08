#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>

class BankAccount {
public:
	BankAccount():balance(0) {}
	void deposit(unsigned int amount);
	void withdraw(unsigned int amount);
private:
	int balance;
	std::mutex mtx;
	std::condition_variable cv;
};

void BankAccount::deposit(unsigned int amount) {
	std::cout << "Trying to deposit..." << std::endl;
	std::lock_guard<std::mutex> lock(mtx);
	balance += amount;
	std::cout << "Deposit $" << amount << std::endl;
      	std::cout << "Balance $" << balance << std::endl;
	cv.notify_one();
}

void BankAccount::withdraw(unsigned int amount) {
	std::cout << "Trying to withdraw..." << std::endl;
	std::unique_lock<std::mutex> lock(mtx);
	// Waiting until balance is greater than the amount to withdraw
	cv.wait(lock, [this, amount] {return balance >= amount;});
	balance -= amount;
	std::cout << "Withdraw $" << amount << std::endl;
	std::cout << "Balance $" << balance << std::endl;
}

int main() {
	BankAccount account;
	std::thread withdrawer([&account](){account.withdraw(500);});
	std::thread depositor([&account](){account.deposit(500);});
	withdrawer.join();
	depositor.join();
	std::cout << "Hello World!" << std::endl;
	return 0;

}

// g++ condition_variable.cpp --pthread
