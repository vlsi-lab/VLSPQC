# VLSPQC

**VLSPQC** (VLSI-Lab Post-Quantum Cryptography) is a collection of clean, hardware-oriented C implementations of the main post-quantum algorithms that have either **passed** or are **still ongoing** in the NIST standardization process.  

The repository is designed to provide **runnable code on FPGA platforms** while remaining compliant with the **Known Answer Test (KAT)** format defined in the original NIST submissions.

---

## 📦 Included Algorithms

This repository currently supports:

- **ML-KEM** (Kyber)  
- **ML-DSA** (Dilithium)  
- **HQC**  
- **Falcon**  
- **SPHINCS+**  
- **CROSS**

---

## 🧪 KAT-Compliant Tests

Each algorithm comes with a **self-contained KAT test**, adapted from the **official reference implementation submitted to NIST**.  

Each test provides:
- **Key generation** (`keygen`)  
- **Encapsulation / Signing** (`encaps` / `sign`)  
- **Decapsulation / Verification** (`decaps` / `verify`)  

This makes the implementations straightforward to validate and benchmark, while ensuring compliance with the standard reference behavior.

---

## 🎯 Purpose

VLSPQC has been developed within the **VLSI-Lab** as a foundation for:
- **Hardware/software co-design** of PQC algorithms.  
- **FPGA-based prototyping and benchmarking.**  
- **Integration into VLSI-oriented PQC accelerators.**  
- **Ensuring reproducibility** against NIST’s official KATs.  

---

## 🔧 Usage

Each algorithm resides in its own directory with a corresponding `Makefile` target. For example:

```sh
make app PROJECT=mlkem
make app PROJECT=mldsa
make app PROJECT=hqc
make app PROJECT=falcon
make app PROJECT=sphincs+
make app PROJECT=cross
```


## 📄 License

This repository follows the licensing terms of the respective reference implementations used as the starting point. Please check individual algorithm directories for specific license details.

## 👥 Authors

- Alessandra Dolmeta    - alessandra.dolmeta@polito.it
- Valeria Piscopo       - valeria.piscopo@polito.it