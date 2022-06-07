Name: elftree
Summary: Tool to display ELF dependencies as a tree
Version: 1.0.0
Release: 0
Group: System/Utilities
License: Apache-2.0
Source0: %{name}-%{version}.tar.gz

BuildRequires:	cmake
BuildRequires:	boost-devel
BuildRequires:	pkgconfig(gmock)
BuildRequires:	pkgconfig(ncurses)
BuildRequires:	pkgconfig(menu)
BuildRequires:	pkgconfig(form)
BuildRequires:	pkgconfig(boost)

%description
This tool is to display so dependency as tree.

%prep
%setup -q

%build
%cmake .
make %{?jobs:-j%jobs}

%check
(cd test && ctest -V)

%install
%make_install

%files
%manifest elftree.manifest
%defattr(-,root,root,-)
%{_bindir}/elftree
%license LICENSE
