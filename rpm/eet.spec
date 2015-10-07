Name         : eet
Version      : 1.7.11
Release      : 1%{?dist}
License      : BSD
Group        : System Environment/Libraries
Provides     : eet = %{version}, libeet1 = %{version}
URL          : http://enlightenment.org/
Packager     : Guillaume Friloux <guillaume@friloux.me>
Summary      : Library for speedy data storage, retrieval, and compression.
Source       : ~/rpmbuild/SOURCES/eet-1.7.11.tar.gz

BuildRequires: pkgconfig automake doxygen m4 autoconf gzip bzip2 tar libjpeg-devel zlib-devel

%description
Eet is a tiny library designed to write an arbitrary set of chunks of
data to a file and optionally compress each chunk (very much like a
zip file) and allow fast random-access reading of the file later
on. It does not do zip as a zip itself has more complexity than is
needed, and it was much simpler to implement this once here.

It also can encode and decode data structures in memory, as well as
image data for saving to eet files or sending across the network to
other machines, or just writing to arbitrary files on the system. All
data is encoded in a platform independent way and can be written and
read by any architecture.


%package devel
Summary      : Eet headers, static libraries, documentation and test programs
Group        : System Environment/Libraries
Requires     : %{name} = %{version}
Requires: libjpeg-devel, zlib-devel

%description devel
Headers, static libraries, test programs and documentation for eet

%prep
rm -rf "$RPM_BUILD_ROOT"
%setup -q

%build
touch config.rpath
./autogen.sh --prefix=/usr                                                     \
             --disable-rpath                                                   \
             --enable-gnutls                                                   \
             --disable-doc                                                     \
             --disable-install-examples                                        \
             --libdir=/usr/lib64

%install
make %{?_smp_mflags}
%makeinstall

%clean
rm -rf "$RPM_BUILD_ROOT"

%post -p /sbin/ldconfig

%postun -p /sbin/ldconfig

%files
%defattr(-,root,root)
%{_libdir}/lib*.so.*

%files devel
%defattr(-, root, root)
%{_libdir}/pkgconfig/*
%{_includedir}/*
%{_libdir}/*.so
%{_libdir}/*.la
%{_libdir}/*.a
