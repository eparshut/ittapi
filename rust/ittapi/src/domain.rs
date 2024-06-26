use crate::util::access_sys_fn;
use std::ffi::CString;

/// A domain enables tagging trace data for different modules or libraries in a program. See the
/// [Domain API] documentation for more information.
///
/// [Domain API]:
///     https://www.intel.com/content/www/us/en/docs/vtune-profiler/user-guide/current/domain-api.html
pub struct Domain(*mut ittapi_sys::__itt_domain);
impl Domain {
    /// Create a new domain. Note that, if the `ittnotify` library is not initialized, this call
    /// will succeed but the domain will be invalid; see discussion TODO.
    ///
    /// ```
    /// # use ittapi::Domain;
    /// let domain = Domain::new("test-domain");
    /// ```
    ///
    /// # Panics
    ///
    /// Panics if the domain name contains a `0` byte.
    #[must_use]
    pub fn new(name: &str) -> Self {
        #[cfg(unix)]
        let create_fn = access_sys_fn!(__itt_domain_create_ptr__3_0);
        #[cfg(windows)]
        let create_fn = access_sys_fn!(__itt_domain_createA_ptr__3_0);
        let c_string =
            CString::new(name).expect("unable to create a CString; does it contain a 0 byte?");
        let domain = unsafe { create_fn(c_string.as_ptr()) };
        Self(domain)
    }

    /// Use the `__itt_domain` pointer internally.
    pub(crate) fn as_ptr(&self) -> *const ittapi_sys::__itt_domain {
        self.0.cast_const()
    }
}

/// As discussed in the [ITT documentation], the `__itt_domain` structure is accessible by any
/// thread in the process.
///
/// [ITT documentation]:
///     https://www.intel.com/content/www/us/en/docs/vtune-profiler/user-guide/current/domain-api.html
unsafe impl Sync for Domain {}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    #[should_panic(expected = "unable to create a CString; does it contain a 0 byte?")]
    fn zero_byte() {
        let _domain = Domain::new("zero\0byte\0name");
    }
}
