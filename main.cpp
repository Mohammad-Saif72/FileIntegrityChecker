
#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include <sstream>
#include <openssl/evp.h>

using namespace std;

const string BASELINE_FILE = "baseline.txt";

// Function 1: Generate SHA-256 hash of a file
bool generateSHA256(const string& filename, string& hash)
{
    ifstream file(filename, ios::binary);

    if (!file.is_open())
    {
        cout << "\nError: Could not open file!\n";
        return false;
    }

    EVP_MD_CTX* context = EVP_MD_CTX_new();

    if (context == nullptr)
    {
        cout << "\nError: Could not create hash context!\n";
        return false;
    }

    if (EVP_DigestInit_ex(context, EVP_sha256(), nullptr) != 1)
    {
        EVP_MD_CTX_free(context);
        cout << "\nError: Hash initialization failed!\n";
        return false;
    }

    char buffer[4096];

    while (file.read(buffer, sizeof(buffer)) || file.gcount() > 0)
    {
        if (EVP_DigestUpdate(
                context,
                buffer,
                static_cast<size_t>(file.gcount())
            ) != 1)
        {
            EVP_MD_CTX_free(context);
            cout << "\nError: Hash update failed!\n";
            return false;
        }
    }

    if (file.bad())
    {
        EVP_MD_CTX_free(context);
        cout << "\nError: File reading failed!\n";
        return false;
    }

    unsigned char digest[EVP_MAX_MD_SIZE];
    unsigned int digestLength = 0;

    if (EVP_DigestFinal_ex(
            context,
            digest,
            &digestLength
        ) != 1)
    {
        EVP_MD_CTX_free(context);
        cout << "\nError: Hash generation failed!\n";
        return false;
    }

    EVP_MD_CTX_free(context);

    stringstream output;

    for (unsigned int i = 0; i < digestLength; i++)
    {
        output << hex << setfill('0')
               << setw(2)
               << static_cast<int>(digest[i]);
    }

    hash = output.str();

    return true;
}


// Function 2: Save trusted hash
void saveBaseline()
{
    string filename;
    string hash;

   
    cout << "\nEnter file path: ";
    getline(cin, filename);

    if (!generateSHA256(filename, hash))
        return;

    ofstream baseline(BASELINE_FILE);

    if (!baseline.is_open())
    {
        cout << "\nError: Cannot save baseline!\n";
        return;
    }

    baseline << filename << '\n';
    baseline << hash << '\n';

    baseline.close();

    cout << "\n====================================\n";
    cout << "     BASELINE CREATED SUCCESSFULLY\n";
    cout << "====================================\n";

    cout << "\nFile: " << filename;
    cout << "\nSHA-256: " << hash << '\n';
}


// Function 3: Verify file integrity
void verifyIntegrity()
{
    ifstream baseline(BASELINE_FILE);

    if (!baseline.is_open())
    {
        cout << "\nNo baseline found!\n";
        cout << "First create a trusted baseline.\n";
        return;
    }

    string filename;
    string originalHash;

    getline(baseline, filename);
    getline(baseline, originalHash);

    baseline.close();

    string currentHash;

    cout << "\nVerifying file: " << filename << '\n';

    if (!generateSHA256(filename, currentHash))
        return;

    cout << "\nOriginal Hash:\n" << originalHash;
    cout << "\n\nCurrent Hash:\n" << currentHash << '\n';

    cout << "\n====================================\n";

    if (originalHash == currentHash)
    {
        cout << "       FILE INTEGRITY VERIFIED\n";
        cout << "       STATUS: FILE UNCHANGED\n";
    }
    else
    {
        cout << "          WARNING!\n";
        cout << "       STATUS: FILE MODIFIED\n";
    }

    cout << "====================================\n";
}


// Function 4: Display menu
void displayMenu()
{
    cout << "\n\n";
    cout << "====================================\n";
    cout << "       FILE INTEGRITY CHECKER\n";
    cout << "          SHA-256 SECURITY\n";
    cout << "====================================\n";

    cout << "1. Generate SHA-256 and save baseline\n";
    cout << "2. Verify file integrity\n";
    cout << "3. Exit\n";

    cout << "====================================\n";
    cout << "Enter your choice: ";
}


// Main function
int main()
{
    int choice;

    do
    {
        displayMenu();

        string input;
        getline(cin, input);

        stringstream parser(input);

        if (!(parser >> choice) || (parser >> ws && !parser.eof()))
        {
            cout << "\nInvalid input! Enter 1, 2 or 3.\n";
            continue;
        }

        switch (choice)
        {
            case 1:
                saveBaseline();
                break;

            case 2:
                verifyIntegrity();
                break;

            case 3:
                cout << "\nThank you for using File Integrity Checker!\n";
                break;

            default:
                cout << "\nInvalid choice! Try again.\n";
        }
        cin.clear();
        fflush(stdin);

    } while (choice != 3);

    return 0;
}
