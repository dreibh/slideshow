Name: slideshow
Version: 2.3.7
Release: 1
Summary: SlideShow HTML image presentation
Group: Applications/Graphics
License: GPL-3.0-or-later
URL: https://www.nntb.no/~dreibh/slideshow/
Source: https://www.nntb.no/~dreibh/slideshow/download/%{name}-%{version}.tar.xz

AutoReqProv: on
BuildRequires: cmake
BuildRequires: gcc
BuildRequires: gcc-c++
BuildRequires: GraphicsMagick-devel
Requires: %{name}-infrastructure = %{version}-%{release}
Recommends: %{name}-example = %{version}-%{release}
BuildRoot: %{_tmppath}/%{name}-%{version}-build

%description
SlideShow is a command-line tool to generate
standards-compliant XHTML-1.1 presentations from a set of
image files. It can be used to easily generate online photo
albums, including JavaScript-based slideshows. Particularly,
this program is intended to be also used in scripts, which
automatically create or update slideshows.

%prep
%setup -q

%build
%cmake -DCMAKE_INSTALL_PREFIX=/usr .
%cmake_build

%install
%cmake_install

%files
%{_bindir}/createslideshow
%{_mandir}/man1/createslideshow.1.gz


%package infrastructure
Summary: SlideShow infrastructure files
BuildArch: noarch

%description infrastructure
The default infrastructure files for SlideShow. These files contain
the button images, the slideshow player frame HTML description and the
the slideshow player JavaScript program.

%files infrastructure
%{_datadir}/slideshow/infrastructure/*.html
%{_datadir}/slideshow/infrastructure/*.js
%{_datadir}/slideshow/infrastructure/*.png


%package example
Summary: SlideShow example
BuildArch: noarch
Recommends: fractgen >= 2.0.6

%description example
An example presentation setup for SlideShow. Use the script
demo-slideshow to generate the HTML presentation from the
example files.

%files example
%{_datadir}/slideshow/example/*.css
%{_datadir}/slideshow/example/*.html
%{_datadir}/slideshow/example/*.png
%{_datadir}/slideshow/example/demo-images/*.fsf
%{_datadir}/slideshow/example/demo-images/*.png
%{_datadir}/slideshow/example/demo-slideshow


%changelog
* Wed Dec 06 2023 Thomas Dreibholz <thomas.dreibholz@gmail.com> - 2.3.7
- New upstream release.
* Sun Jan 22 2023 Thomas Dreibholz <thomas.dreibholz@gmail.com> - 2.3.6
- New upstream release.
* Sun Sep 11 2022 Thomas Dreibholz <dreibh@iem.uni-due.de> - 2.3.5
- New upstream release.
* Tue Nov 30 2021 Thomas Dreibholz <dreibh@iem.uni-due.de> - 2.3.4
- New upstream release.
* Fri Nov 13 2020 Thomas Dreibholz <dreibh@iem.uni-due.de> - 2.3.3
- New upstream release.
* Fri Nov 13 2020 Thomas Dreibholz <dreibh@iem.uni-due.de> - 2.3.2
- New upstream release.
* Fri Feb 07 2020 Thomas Dreibholz <dreibh@iem.uni-due.de> - 2.3.2
- New upstream release.
* Wed Aug 07 2019 Thomas Dreibholz <dreibh@iem.uni-due.de> - 2.3.1
- New upstream release.
* Fri Jul 26 2019 Thomas Dreibholz <dreibh@iem.uni-due.de> - 2.3.0
- New upstream release.
* Wed Nov 22 2017 Thomas Dreibholz <dreibh@iem.uni-due.de> - 1.0.0
- Initial RPM release
