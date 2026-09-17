<div align="center">
  <img src="https://via.placeholder.com/150/10B981/FFFFFF?text=Axi" alt="Axi Logo" width="150" height="150" />
  <h1>Axi Language</h1>
  <p><strong>The declarative orchestration layer of the modern DAG ecosystem.</strong></p>
  
  [![License: AGPL v3](https://img.shields.io/badge/License-AGPL_v3-blue.svg)](https://www.gnu.org/licenses/agpl-3.0) [![License: Commercial](https://img.shields.io/badge/License-Commercial-indigo.svg)](https://allos.the-ethos-project.com)
  [![Version: 1.0.0-beta](https://img.shields.io/badge/Version-1.0.0--beta-blue.svg)](https://axi.the-ethos-project.com)
  [![Status](https://img.shields.io/badge/Status-Active-success.svg)]()
</div>

<br/>

Axi is a powerful orchestration and version control language that sits at the base of the modern DAG ecosystem. It serves as the definitive declarative language for the Axi DVCS. general-purpose programming capabilities for the community are written in **Axos** (.axos) and encapsulated safely inside the open-source **Axi Virtual Machine (AVM)**.

## 🚀 The Axi Architecture: DVCS & AVM

Axi is uniquely designed with firm boundaries between orchestration and execution:

* **Axi (The DVCS Language Layer):** The .axi language is used strictly as a declarative, domain-specific language for Distributed Version Control (like Git configs or GitHub Actions). It controls DAG routing, orchestration, and engine configurations.
* **Axos & The Axi Virtual Machine (AVM):** We've migrated the original bare-metal C compiler capabilities into a secure, bare-bones Virtual Machine. When you need to execute general-purpose logic, it runs safely inside the AVM runtime.

### Firm Boundary: Axi vs. Allos
Axi is fully open-source under the AGPLv3 for the community. In contrast, **Allos** is our proprietary enterprise edition—an advanced superset used exclusively for commercial applications, internal AI cognitive routing, and proprietary IP. Both languages are natively tracked by the .axi DVCS DAG, but their execution environments remain strictly separate.

## 📁 Repository Structure

* /avm - The bare-bones Community Axi Virtual Machine (formerly xi_compiler).
* /axi_lang - Standard library and DVCS core language specifications.
* /axi_ide - The official IDE for Axi orchestration.
* /axi - Core .axi distributed version control backend (DAG & YAML engine).
* /components - Internal subsystems and libraries.

## 🤝 Contributing

Axi is proudly open-source and built for the community. We welcome contributions of all sizes! Please read our [CONTRIBUTING.md](CONTRIBUTING.md) for details on our code of conduct, and the process for submitting pull requests.

## 🛡️ Security

If you discover a security vulnerability, please review our [SECURITY.md](SECURITY.md) guidelines. Do not open a public issue for security-related flaws.

## 🏢 Enterprise Support

Axi is stewarded by **The Ethos Project, LLC.** 
If your business requires proprietary tooling, advanced security protocols, or dedicated Service Level Agreements (SLAs), explore our enterprise ecosystem:
👉 [Allos Enterprise](https://allos.the-ethos-project.com)

## 📄 License

Axi operates under a **Dual / Mixed Licensing** model to support both the open-source community and enterprise businesses.

* **Open Source (AGPLv3):** This repository is licensed under the [GNU Affero General Public License v3.0](LICENSE). You are completely free to use, modify, and distribute Axi for open-source projects, provided that you also open-source your derivative work under the same terms.
* **Commercial / Enterprise:** If you intend to use Axi in a proprietary, closed-source application or require advanced transpilation capabilities without adhering to the AGPL obligations, you must obtain a commercial license via [Allos Enterprise](https://allos.the-ethos-project.com).

