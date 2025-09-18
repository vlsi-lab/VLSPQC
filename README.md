# VLSPQC

**VLSPQC** (VLSI-Lab Post-Quantum Cryptography) is a collection of clean, hardware-oriented C implementations of the main post-quantum algorithms that have either **passed** or are **still ongoing** in the NIST standardization process.  

The repository is designed to provide **runnable code on FPGA platforms** while remaining compliant with the **Known Answer Test (KAT)** format defined in the original NIST submissions.

---

## 📦 Included Algorithms

This repository currently supports:

- **ML-KEM**    [Kyber](https://github.com/pq-crystals/kyber)  
This repository contains the official reference implementation of the [Kyber](https://www.pq-crystals.org/kyber/) key encapsulation mechanism.
Kyber has been selected for standardization in [round 3](https://csrc.nist.gov/Projects/post-quantum-cryptography/round-3-submissions) 
of the [NIST PQC](https://csrc.nist.gov/projects/post-quantum-cryptography) standardization project.


- **ML-DSA**    [Dilithium](https://github.com/pq-crystals/dilithium)  
This repository contains the official reference implementation of the [Dilithium](https://www.pq-crystals.org/dilithium/) signature scheme.
Dilithium is standardized as [FIPS 204](https://csrc.nist.gov/pubs/fips/204/final).

- **HQC**       [HQC](https://gitlab.com/pqc-hqc/hqc/)
    - **HQC-2024**      : previous version of HQC (23/03/2024)
    - **HQC-2025**      : last version of HQC (22/08/2025)
    - **HQC-2025-v5**   : last version of HQC (22/08/2025) - additional modification to Karatsuba multiplication, for hardware optimization.  
This repository provides the official implementation of [HQC](https://pqc-hqc.org), a code-based Key Encapsulation Mechanism (KEM) whose security is based on the hardness of solving the Quasi-Cylic Syndrome Decoding (QCSD) problem. HQC is one of the selected algorithms from the [NIST's Post-Quantum Cryptography Standardization Project](https://csrc.nist.gov/projects/post-quantum-cryptography).

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

## 🔧 Usage - TBD

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