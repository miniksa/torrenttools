%global forgeurl https://www.github.com/fbdtemme/torrenttools
%global tag      0.1.3
Version:         %{tag}

# Verbose processing. Remove -i -v before commit
%forgemeta

Name:            torrenttools
URL:             %{forgeurl}
Source:          %{forgesource}
License:         MIT
Release:         1%{?dist}
Summary:         A commandline tool for creating, inspecting and modifying bittorrent metafiles.

BuildRequires:   openssl-devel
BuildRequires:   cmake
BuildRequires:   make
BuildRequires:   git

%{?fedora:BuildRequires:   gcc-c++ >= 10.0.0}
%{?fedora:BuildRequires:   libstdc++-devel >= 10.0.0}
%{?fedora:Requires:        libstdc++ >= 10.0.0}

%{?el8:BuildRequires:      gcc-toolset-10}
%{?el8:BuildRequires:      epel-release}

Requires:            openssl >= 1.0.0

%description
A commandline tool for creating, inspecting and modifying bittorrent metafiles.

%prep
# Verbose processing. Remove -v before commit
%forgesetup

%build

%if 0%{?el8}
source /opt/rh/gcc-toolset-10/enable
%endif

%cmake -DBUILD_SHARED_LIBS=OFF -DCMAKE_BUILD_TYPE=RelWithDebInfo

%if 0%{?el8}
%cmake_build . torrenttools
%else
%cmake_build --target torrenttools
%endif

%install

%if 0%{?el8}
DESTDIR="%{buildroot}" %__cmake  --install . --component torrenttools
%else
%cmake_install --component torrenttools
%endif

%files
%defattr(-,root,root,-)
%dir %{_sysconfdir}/torrenttools
%{_sysconfdir}/torrenttools/config.yaml
%{_sysconfdir}/torrenttools/trackers.json
%{_bindir}/torrenttools

%changelog
