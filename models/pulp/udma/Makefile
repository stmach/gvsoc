pulp/udma/udma_v$(udma/version)_$(pulp_chip_family)_impl_CFLAGS += -DUDMA_VERSION=$(udma/version)

ifeq '$(udma/version)' '2'
IMPLEMENTATIONS += pulp/udma/udma_v2_$(pulp_chip_family)_impl
COMPONENTS += pulp/udma/udma_v2
pulp/udma/udma_v2_$(pulp_chip_family)_impl_SRCS += pulp/udma/udma_v2_impl.cpp
pulp/udma/udma_v2_$(pulp_chip_family)_impl_SRCS += pulp/udma/udma_uart_v1.cpp
endif

ifeq '$(udma/version)' '3'
IMPLEMENTATIONS += pulp/udma/udma_v3_$(pulp_chip_family)_impl
COMPONENTS += pulp/udma/udma_v3
pulp/udma/udma_v3_$(pulp_chip_family)_impl_SRCS += pulp/udma/udma_v3_impl.cpp
pulp/udma/udma_v3_$(pulp_chip_family)_impl_SRCS += pulp/udma/udma_uart_v1.cpp
endif

ifeq '$(udma/spim/version)' '2'
pulp/udma/udma_v$(udma/version)_$(pulp_chip_family)_impl_CFLAGS += -DHAS_SPIM
pulp/udma/udma_v$(udma/version)_$(pulp_chip_family)_impl_SRCS += pulp/udma/udma_spim_v2.cpp
endif

ifeq '$(udma/spim/version)' '3'
pulp/udma/udma_v$(udma/version)_$(pulp_chip_family)_impl_CFLAGS += -DHAS_SPIM
pulp/udma/udma_v$(udma/version)_$(pulp_chip_family)_impl_SRCS += pulp/udma/udma_spim_v3.cpp
endif

ifeq '$(udma/cpi/version)' '1'
pulp/udma/udma_v$(udma/version)_$(pulp_chip_family)_impl_CFLAGS += -DHAS_CPI
pulp/udma/udma_v$(udma/version)_$(pulp_chip_family)_impl_SRCS += pulp/udma/udma_cpi_v1.cpp
endif

ifeq '$(udma/hyper/version)' '1'
pulp/udma/udma_v$(udma/version)_$(pulp_chip_family)_impl_CFLAGS += -DHAS_HYPER
pulp/udma/udma_v$(udma/version)_$(pulp_chip_family)_impl_SRCS += pulp/udma/udma_hyper_v1.cpp
endif

ifeq '$(udma/i2c/version)' '2'
pulp/udma/udma_v$(udma/version)_$(pulp_chip_family)_impl_CFLAGS += -DHAS_I2C
pulp/udma/udma_v$(udma/version)_$(pulp_chip_family)_impl_SRCS += pulp/udma/udma_i2c_v2.cpp
endif
