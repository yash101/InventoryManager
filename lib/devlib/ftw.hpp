#ifndef FTW_HPP
#define FTW_HPP
#include <string>
#include <vector>
namespace dev
{
    enum member_t
    {
        directory = 0,
        file = 1
    };

    class directory_member
    {
    private:
        member_t _type;
        std::string _location;
    public:
        member_t type() const { return _type; }
        std::string path() const { return _location; }
    };

    std::vector<dev::directory_member> tree(std::string location);
}
#endif // FTW_H
