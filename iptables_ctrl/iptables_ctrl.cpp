#include <iostream>
#include <vector>


class IptablesCtrl
{
public:
    //! Default constructor
    IptablesCtrl();

    //! Copy constructor
    IptablesCtrl(const IptablesCtrl &other);

    //! Move constructor
    IptablesCtrl(IptablesCtrl &&other) noexcept;

    //! Destructor
    virtual ~IptablesCtrl() noexcept;

    //! Copy assignment operator
    IptablesCtrl& operator=(const IptablesCtrl &other);

    //! Move assignment operator
    IptablesCtrl& operator=(IptablesCtrl &&other) noexcept;



protected:

private:

};


int main()
{
    std::cout << "Hello world!" << "\n";
    return 0;
}
