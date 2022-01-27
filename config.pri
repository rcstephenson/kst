# Define if you have cfitsio - not used on unix
HAVE_CFITSIO = 0
# Set to 1 to disable usage of cfitsio - only for unix
DISABLE_CFITSIO = 0

#Define if you have dirfile getdata library version 0.4.2 or greater required. - not used on unix
HAVE_DIRFILE = 0
# Set to 1 to disable usage of dirfile - only for unix
DISABLE_DIRFILE = 0

#Define if you have the gsl library installed - not used on unix
HAVE_GSL = 0
# Set to 1 to disable usage of gsl - only for unix
DISABLE_GSL = 0


defineTest(LibExists) {
  lib = $$ARGS

unix { 
  !mac {
  contains(lib, gsl) {
    contains(DISABLE_GSL, 1) {
      # Overridden, do not check.
      return(false)
    }
  }
  contains(lib, cfitsio) {
    contains(DISABLE_CFITSIO, 1) {
      # Overridden, do not check.
      return(false)
    }
  }
  contains(lib, getdata) {
    contains(DISABLE_DIRFILE, 1) {
      # Overridden, do not check.
      return(false)
    }
  }
  PKGCONFIGRESULT = $$system(pkg-config --libs $$lib)
  !contains(PKGCONFIGRESULT, -l$$lib) {
    message(Could not find $$lib disabling associated features)
    return(false)
  }
  message(Found $$lib at $$PKGCONFIGRESULT)
  return(true)
} }


contains(lib, gsl) {
  GSLDIR = $$(GSLDIR)
  win32:!isEmpty(GSLDIR) {
    return(true)
  }
  !win32:contains(HAVE_GSL, 1) {
    return(true)
  }
  return(false)
}

contains(lib, cfitsio) {
  contains(HAVE_CFITSIO, 1) {
    return(true)
  }
  return(false)
}

contains(lib, getdata) {
  !win32:contains(HAVE_DIRFILE, 1) {
    return(true)
  }
  GETDATADIR = $$(GETDATADIR)
  win32:!isEmpty(GETDATADIR) {
    return(true)
  }
  macx:!isEmpty(GETDATADIR) {
    return(true)
  }
  return(false)
}

contains(lib, netcdf) {
  NETCDFDIR = $$(NETCDFDIR)
  win32:!isEmpty(NETCDFDIR) {
    return(true)
  }
  !win32:contains(HAVE_NETCDFDIR, 1) {
    return(true)
  }
  return(false)
}


}


defineReplace(pkginclude) {
    PKGCONFIG_INCLUDE_DIR = $$system(pkg-config --variable includedir $$1)
    message(Using $$1 headers in $$PKGCONFIG_INCLUDE_DIR)
    return($$PKGCONFIG_INCLUDE_DIR)
}

